/*
 * _iconv_codec.c: Universal Unicode Codec for iconv
 *
 * Copyright (C) 2003 Hye-Shik Chang. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: _iconv_codec.c,v 1.2 2003/11/27 09:18:05 perky Exp $
 */

#include "Python.h"
#include <iconv.h>
#include "_iconv_codec_compat.h"

#ifdef Py_USING_UNICODE
# if Py_UNICODE_SIZE == 2
#  define MBENCODED_LENGTH_MAX      4
#  define _Py_UNICODE_SWAP(c)       (Py_UNICODE)((c)>>8 | (c)<<8)
# elif Py_UNICODE_SIZE == 4
#  define MBENCODED_LENGTH_MAX      6
#  define _Py_UNICODE_SWAP(c)       (Py_UNICODE)((c)>>24 | \
                                     ((c)&0x00ff0000)>>8 | \
                                     ((c)<0x0000ff00)<<8 | (c)<<24)
# endif
#else
# error "Unicode is not available"
#endif

#ifdef WORDS_BIGENDIAN
#define ENDIANSUFX          "BE"
#else
#define ENDIANSUFX          "LE"
#endif

#if defined(__GNU_LIBRARY__) || defined(__hpux)
typedef char **iconv_arg2_t;
#else
typedef const char **iconv_arg2_t;
#endif

#ifndef ucs4_t
# ifdef uint32_t
typedef uint32_t ucs4_t;
# else
typedef unsigned long ucs4_t;
# endif
#endif

#define ERROR_STRICT        (PyObject *)(1)
#define ERROR_IGNORE        (PyObject *)(2)
#define ERROR_REPLACE       (PyObject *)(3)
#define ERROR_MAX           ERROR_REPLACE

typedef enum {
    UNIINTERNAL_DONTUSE = 0,
    UNIINTERNAL_UCS,
    UNIINTERNAL_UCS_SWAPPED,
    UNIINTERNAL_UTF_8,
} uniinternal_type_t;

#ifndef LACKS_PYSTRING_FROMFORMAT
static const char *uniinternal_type_names[] = {
    "unusable", "UCS", "UCS(SWAPPED)", "UTF-8",
};
#endif

static struct {
    const char          *encoding;
    uniinternal_type_t   type;
} uniinternal_modes[] = {
#if Py_UNICODE_SIZE == 2
/* Py_UNICODE* may contain surrogate characters */
    {"UTF-16" ENDIANSUFX, UNIINTERNAL_UCS},
    {"UTF16" ENDIANSUFX, UNIINTERNAL_UCS},
#else
    {"UCS-4-INTERNAL", UNIINTERNAL_UCS}, /* GNU libiconv, FreeBSD, APR */
    {"UCS-4" ENDIANSUFX, UNIINTERNAL_UCS}, /* SunOS */
    {"UCS-4", UNIINTERNAL_UCS}, /* GLIBC */
#endif
    {"UTF-8", UNIINTERNAL_UTF_8}, /* SunOS(CJK) */
    {"\0", UNIINTERNAL_DONTUSE},
};

typedef struct {
    const Py_UNICODE    *inbuf, *inbuf_top, *inbuf_end;
    const unsigned char *rinbuf, *rinbuf_top, *rinbuf_end;
                        /* the 'roasted' inbuf: for swapped, utf-8 */
    unsigned char       *outbuf, *outbuf_end;
    PyObject            *excobj, *outobj;
} IconvEncoderBuffer;

typedef struct {
    const unsigned char *inbuf, *inbuf_top, *inbuf_end;
    Py_UNICODE          *outbuf, *outbuf_end;
    PyObject            *excobj, *outobj;
} IconvDecoderBuffer;

typedef struct {
    PyObject_HEAD
    char        *encoding;
    const char  *unicode_encoding;
    int          unitype;
} IconvEncoderObject;

typedef struct {
    PyObject_HEAD
    char        *encoding;
    const char  *unicode_encoding;
    int          unitype;
    size_t      (*iconvwrap)(
                    iconv_t cd, iconv_arg2_t inbuf, size_t *inbytesleft,
                    char **outbuf, size_t *outbytesleft);
    size_t      (*iconvwrap2)(iconv_t cd, IconvDecoderBuffer *buf,
                    size_t inleft, size_t outleft);
                /* an alternative wrapper: for utf8 backend */
} IconvDecoderObject;

#define MAXPENDING      64
typedef struct {
    PyObject_HEAD
    IconvDecoderObject  *codec;
    iconv_t              ic;
    unsigned char        pending[MAXPENDING];
    size_t               pendingsize;
    PyObject            *stream, *errors;
} IconvStreamReaderObject;

typedef struct {
    PyObject_HEAD
    IconvEncoderObject  *codec;
    iconv_t              ic;
    PyObject            *stream, *errors;
} IconvStreamWriterObject;

static char *kwarglist[] = {"input", "errors", NULL};

static PyObject *iconvencoder_encode(IconvEncoderObject *, iconv_t,
                            const Py_UNICODE *, int, PyObject *, int);
static PyObject *iconvstreamreader_create(IconvDecoderObject *,
                            PyObject *, const char *);
static PyObject *iconvstreamwriter_create(IconvEncoderObject *,
                            PyObject *, const char *);

#define LOGICERROR() \
            PyErr_SetString(PyExc_RuntimeError, "internal logic error")

PyDoc_STRVAR(iconvencoder_doc,
"I.encode(unicode, [,errors]) -> (string, length consumed)\n\
\n\
Return an encoded string version of `unicode'. errors may be given to\n\
set a different error handling scheme. Default is 'strict' meaning that\n\
encoding errors raise a UnicodeEncodeError. Other possible values are\n\
'ignore', 'replace' and 'xmlcharrefreplace' as well as any other name\n\
registered with codecs.register_error that can handle UnicodeEncodeErrors.");

PyDoc_STRVAR(iconvdecoder_doc,
"I.decode(string, [,errors]) -> (unicodeobject, length consumed)\n\
\n\
Decodes `string' using I, an IconvDecoder instance. errors may be given\n\
to set a different error handling scheme. Default is 'strict' meaning\n\
that encoding errors raise a UnicodeDecodeError. Other possible values\n\
are 'ignore' and 'replace' as well as any other name registerd with\n\
codecs.register_error that is able to handle UnicodeDecodeErrors.");


static PyObject *
make_tuple(PyObject *unicode, int len)
{
    PyObject        *v, *w;

    v = PyTuple_New(2);
    if (v == NULL) {
        Py_DECREF(unicode);
        return NULL;
    }
    PyTuple_SET_ITEM(v, 0, unicode);

    w = PyInt_FromLong(len);
    if (w == NULL) {
        Py_DECREF(v);
        return NULL;
    }
    PyTuple_SET_ITEM(v, 1, w);

    return v;
}

static PyObject *
get_errorcallback(const char *errors)
{
    if (errors == NULL || strcmp(errors, "strict") == 0)
        return ERROR_STRICT;
    else if (strcmp(errors, "ignore") == 0)
        return ERROR_IGNORE;
    else if (strcmp(errors, "replace") == 0)
        return ERROR_REPLACE;
    else
#ifndef LACKS_ERROR_CALLBACKS
        return PyCodec_LookupError(errors);
#else
        return NULL;
#endif
}

#define UTF8NEXTCHAR(p)             \
    if (*(p) < 0x80) (p)++;         \
    else if (*(p) < 0xe0) (p) += 2; \
    else if (*(p) < 0xf0) (p) += 3; \
    else if (*(p) < 0xf8) (p) += 4; \
    else if (*(p) < 0xfc) (p) += 5; \
    else if (*(p) < 0xfe) (p) += 6; \
    else (p)++;

static const unsigned char *
skipchars_utf8(const unsigned char *st, int n)
{
    int  i;

    for (i = 0; i < n; i++)
        UTF8NEXTCHAR(st)

    return st;
}

static int
countchars_utf8(const unsigned char *st, const unsigned char *en)
{
    int      chars;

    for (chars = 0; st < en; chars++)
        UTF8NEXTCHAR(st)

    return chars;
}

static int
expand_encodebuffer(IconvEncoderBuffer *buf, int esize)
{
    int      orgpos, orgsize;

    orgpos  = (int)((char*)buf->outbuf - PyString_AS_STRING(buf->outobj));
    orgsize = PyString_GET_SIZE(buf->outobj);
    if (_PyString_Resize(&buf->outobj, orgsize + (
                esize < (orgsize >> 1) ? (orgsize >> 1) | 1 : esize)) == -1)
        return -1;

    buf->outbuf = (unsigned char *)PyString_AS_STRING(buf->outobj) + orgpos;
    buf->outbuf_end = (unsigned char *)PyString_AS_STRING(buf->outobj)
                        + PyString_GET_SIZE(buf->outobj);

    return 0;
}
#define RESERVE_ENCODEBUFFER(buf, s) {                          \
    if ((s) < 1 || (buf)->outbuf + (s) > (buf)->outbuf_end)     \
        if (expand_encodebuffer(buf, s) == -1)                  \
            goto errorexit;                                     \
}

static int
iconvencoder_error(IconvEncoderObject *self,
                   iconv_t ic, IconvEncoderBuffer *buf,
                   PyObject *errors, int esize)
{
#ifndef LACKS_ERROR_CALLBACKS
    PyObject    *retobj = NULL, *retstr = NULL;
    PyObject    *argsobj, *tobj;
    int          retstrsize, newpos;
#endif
    const char  *reason;
    int          start, end;

    if (errno == E2BIG) {
        RESERVE_ENCODEBUFFER(buf, -1);
        return 0; /* retry it */
    }

    switch (errno) {
#if __CYGWIN__
    case ENOENT: /* FALLTHROUGH */
#endif
    case EILSEQ:
        reason = "illegal multibyte sequence"; break;
    case EINVAL:
        reason = "incomplete multibyte sequence"; break;
    default:
        PyErr_SetString(PyExc_RuntimeError,
                        "unknown runtime error from iconv");
        return -1;
    }

    if (errors == ERROR_REPLACE) {
        Py_UNICODE   replchar;
        char        *inbuf = (char *)&replchar;
        size_t       r, outleft, inleft;

        switch (self->unitype) {
        case UNIINTERNAL_UCS:
            replchar = '?';
            inleft = Py_UNICODE_SIZE;
            break;
        case UNIINTERNAL_UCS_SWAPPED:
            replchar = _Py_UNICODE_SWAP((Py_UNICODE)'?');
            inleft = Py_UNICODE_SIZE;
            break;
        case UNIINTERNAL_UTF_8:
            inleft = 1;
            *inbuf = '?';
            break;
        default:
            LOGICERROR();
            return -1;
        }

        for (;;) {
            outleft = (size_t)(buf->outbuf_end - buf->outbuf);
            r = iconv(ic, (iconv_arg2_t)&inbuf, &inleft,
                          (char **)&(buf->outbuf), &outleft);
            if (r == (size_t)-1 && errno == E2BIG) {
                RESERVE_ENCODEBUFFER(buf, -1);
                continue;
            } else
                break;
        }

        if (r == (size_t)-1) {
            RESERVE_ENCODEBUFFER(buf, 1);
            *buf->outbuf++ = '?';
        }
    }

    if (errors == ERROR_IGNORE || errors == ERROR_REPLACE) {
        buf->inbuf += esize;
        switch (self->unitype) {
        case UNIINTERNAL_UCS_SWAPPED:
            buf->rinbuf += esize * Py_UNICODE_SIZE;
            break;
        case UNIINTERNAL_UTF_8:
            buf->rinbuf = skipchars_utf8(buf->rinbuf, esize);
            break;
        default:
            break;
        }
        return 0;
    }

    start = (int)(buf->inbuf - buf->inbuf_top);
    end = start + esize;

#ifdef LACKS_ERROR_CALLBACKS
    if (esize == 1)
        PyErr_Format(PyExc_UnicodeError,
#if Py_UNICODE_SIZE == 2
            "'%s' codec can't encode byte '\\u%04x' in position %d: %s",
#else
            "'%s' codec can't encode byte '\\U%08lx' in position %d: %s",
#endif
            self->encoding, *buf->inbuf, start, reason);
    else
        PyErr_Format(PyExc_UnicodeError,
            "'%s' codec can't encode bytes in position %d-%d: %s",
            self->encoding, start, end, reason);
errorexit:
    return -1;
#else
    /* use cached exception object if available */
    if (buf->excobj == NULL) {
        buf->excobj = PyUnicodeEncodeError_Create(self->encoding,
                        buf->inbuf_top, (int)(buf->inbuf_end - buf->inbuf_top),
                        start, end, reason);
        if (buf->excobj == NULL)
            goto errorexit;
    } else
        if (PyUnicodeEncodeError_SetStart(buf->excobj, start) ||
            PyUnicodeEncodeError_SetEnd(buf->excobj, end) ||
            PyUnicodeEncodeError_SetReason(buf->excobj, reason))
                goto errorexit;

    if (errors == ERROR_STRICT) {
        PyCodec_StrictErrors(buf->excobj);
        goto errorexit;
    }

    argsobj = PyTuple_New(1);
    if (argsobj == NULL)
        goto errorexit;

    PyTuple_SET_ITEM(argsobj, 0, buf->excobj);
    Py_INCREF(buf->excobj);
    retobj = PyObject_CallObject(errors, argsobj);
    Py_DECREF(argsobj);
    if (retobj == NULL)
        goto errorexit;

    if (!PyTuple_Check(retobj) || PyTuple_GET_SIZE(retobj) != 2 ||
        !PyUnicode_Check((tobj = PyTuple_GET_ITEM(retobj, 0))) ||
        !PyInt_Check(PyTuple_GET_ITEM(retobj, 1))) {
        PyErr_SetString(PyExc_ValueError,
                "encoding error handler must return (unicode, int) tuple");
        goto errorexit;
    }

    retstr = iconvencoder_encode(self, ic, PyUnicode_AS_UNICODE(tobj),
                    PyUnicode_GET_SIZE(tobj), ERROR_STRICT, 0);
    if (retstr == NULL)
        goto errorexit;

    retstrsize = PyString_GET_SIZE(retstr);
    RESERVE_ENCODEBUFFER(buf, retstrsize);

    memcpy(buf->outbuf, PyString_AS_STRING(retstr), retstrsize);
    buf->outbuf += retstrsize;

    newpos = (int)PyInt_AS_LONG(PyTuple_GET_ITEM(retobj, 1));
    if (newpos < 0)
        newpos += (int)(buf->inbuf_end - buf->inbuf_top);
    if (newpos < 0 || buf->inbuf_top + newpos > buf->inbuf_end) {
        PyErr_Format(PyExc_IndexError,
            "position %d from error handler out of bounds", newpos);
        goto errorexit;
    }
    switch (self->unitype) {
    case UNIINTERNAL_UTF_8:
        if (newpos >= start) /* buf->rinbuf is at 'start' position now */
            buf->rinbuf = skipchars_utf8(buf->rinbuf, newpos - start);
        else
            buf->rinbuf = skipchars_utf8(buf->rinbuf_top, newpos);
        break;
    case UNIINTERNAL_UCS_SWAPPED:
        buf->rinbuf = buf->rinbuf_top + newpos * Py_UNICODE_SIZE;
        break;
    default:
        break;
    }
    buf->inbuf = buf->inbuf_top + newpos;

    Py_DECREF(retobj);
    Py_DECREF(retstr);
    return 0;

errorexit:
    Py_XDECREF(retobj);
    Py_XDECREF(retstr);
    return -1;
#endif
}

static int
iconvencoder_conv(IconvEncoderObject *self,
                  iconv_t ic, IconvEncoderBuffer *buf,
                  PyObject *errors)
{
    size_t   r;

    for (;;) {
        const unsigned char *inbuf_bf;
        size_t   inleft, outleft;

        outleft = (size_t)(buf->outbuf_end - buf->outbuf);

        switch (self->unitype) {
        case UNIINTERNAL_UCS:
            inleft = (size_t)buf->inbuf_end - (size_t)buf->inbuf;
            r = iconv(ic, (iconv_arg2_t)&(buf->inbuf), &inleft,
                          (char **)&(buf->outbuf), &outleft);
            break;
        case UNIINTERNAL_UCS_SWAPPED:
            inleft = (int)(buf->rinbuf_end - buf->rinbuf);
            inbuf_bf = buf->rinbuf;
            r = iconv(ic, (iconv_arg2_t)&(buf->rinbuf), &inleft,
                          (char **)&(buf->outbuf), &outleft);
            buf->inbuf += (size_t)(buf->rinbuf - inbuf_bf) / Py_UNICODE_SIZE;
            break;
        case UNIINTERNAL_UTF_8:
            inleft = (int)(buf->rinbuf_end - buf->rinbuf);
            inbuf_bf = buf->rinbuf;
            r = iconv(ic, (iconv_arg2_t)&(buf->rinbuf), &inleft,
                          (char **)&(buf->outbuf), &outleft);
            buf->inbuf += countchars_utf8(inbuf_bf, buf->rinbuf);
            break;
        default:
            LOGICERROR();
            return -1;
        }

        if (r != (size_t)-1)
            return 0;
        else if (iconvencoder_error(self, ic, buf, errors, 1))
            return -1;
        else if (buf->inbuf >= buf->inbuf_end)
            return 0;
    }
}

static int
iconvencoder_flush(IconvEncoderObject *self,
                   iconv_t ic, IconvEncoderBuffer *buf,
                   PyObject *errors)
{
    size_t   r, outleft;

    for (;;) {
        outleft = (size_t)(buf->outbuf_end - buf->outbuf);

        r = iconv(ic, NULL, NULL, (char **)&(buf->outbuf), &outleft);
        if (r == (size_t)-1) {
            if (errno == E2BIG) {
                RESERVE_ENCODEBUFFER(buf, -1);
                continue;
            } else {
                if (iconvencoder_error(self, ic, buf, errors, 0))
                    goto errorexit;
            }
        }
        break;
    }

    return 0;

errorexit:
    return -1;
}

static int
iconvencoder_prepbuf(IconvEncoderBuffer *buf,
                     const Py_UNICODE *data, int datalen,
                     int unitype)
{
    unsigned char   *rinbuf;

    buf->excobj = NULL;
    buf->rinbuf_top = NULL;
    buf->inbuf = buf->inbuf_top = data;
    buf->inbuf_end = buf->inbuf_top + datalen;
    buf->outobj = PyString_FromStringAndSize(NULL, datalen * 2 + 16);
    if (buf->outobj == NULL)
        return -1;
    buf->outbuf = (unsigned char *)PyString_AS_STRING(buf->outobj);
    buf->outbuf_end = buf->outbuf + PyString_GET_SIZE(buf->outobj);

    switch (unitype) {
    case UNIINTERNAL_UCS_SWAPPED:
        rinbuf = PyMem_Malloc(datalen * Py_UNICODE_SIZE);
        if (rinbuf == NULL)
            return -1;
        buf->rinbuf_top = buf->rinbuf = rinbuf;
        buf->rinbuf_end = buf->rinbuf_top + datalen * Py_UNICODE_SIZE;
        {
            Py_UNICODE  *rbufu = (Py_UNICODE *)rinbuf;

            for (; buf->inbuf < buf->inbuf_end; buf->inbuf++)
                *rbufu++ = _Py_UNICODE_SWAP(*buf->inbuf);
        }
        buf->inbuf = buf->inbuf_top;
        break;
    case UNIINTERNAL_UTF_8:
        rinbuf = PyMem_Malloc(datalen * MBENCODED_LENGTH_MAX);
        if (rinbuf == NULL)
            return -1;
        buf->rinbuf_top = buf->rinbuf = rinbuf;
        for (; buf->inbuf < buf->inbuf_end; buf->inbuf++) {
            ucs4_t       code = *buf->inbuf;
            int          size;

            if (code < 0x80) size = 1;
            else if (code < 0x800) size = 2;
            else {
#if Py_UNICODE_SIZE == 2
                /* Unfold a Surrogate-Pair */
                if (code >= 0xd800 && code < 0xdc00 &&
                         buf->inbuf+1 < buf->inbuf_end &&
                         buf->inbuf[1] >= 0xdc00 &&
                         buf->inbuf[1] < 0xe000) {
                    code = 0x10000 + ((code - 0xd800) << 10) +
                                (buf->inbuf[1] - 0xdc00);
                    buf->inbuf++;
                }
#endif
                if (code < 0x10000) size = 3;
                else if (code < 0x200000) size = 4;
                else if (code < 0x4000000) size = 5;
                else size = 6;
            }

            switch (size) {
            case 6:
                rinbuf[5] = 0x80 | (code & 0x3f);
                code = code >> 6;
                code |= 0x4000000;
                /* FALLTHROUGH */
            case 5:
                rinbuf[4] = 0x80 | (code & 0x3f);
                code = code >> 6;
                code |= 0x200000;
                /* FALLTHROUGH */
            case 4:
                rinbuf[3] = 0x80 | (code & 0x3f);
                code = code >> 6;
                code |= 0x10000;
                /* FALLTHROUGH */
            case 3:
                rinbuf[2] = 0x80 | (code & 0x3f);
                code = code >> 6;
                code |= 0x800;
                /* FALLTHROUGH */
            case 2:
                rinbuf[1] = 0x80 | (code & 0x3f);
                code = code >> 6;
                code |= 0xc0;
                /* FALLTHROUGH */
            case 1:
                rinbuf[0] = code;
            }

            rinbuf += size;
        }
        buf->inbuf = buf->inbuf_top;
        buf->rinbuf_end = rinbuf;
        break;
    default:
        break;
    }

    return 0;
}

static PyObject *
iconvencoder_encode(IconvEncoderObject *self,
                    iconv_t ic, const Py_UNICODE *data, int datalen,
                    PyObject *errors, int doflush)
{
    IconvEncoderBuffer   buf;
    int                  finalsize;

    if (datalen == 0)
        return PyString_FromString("");

    if (iconvencoder_prepbuf(&buf, data, datalen, self->unitype) == -1)
        goto errorexit;

    if (iconvencoder_conv(self, ic, &buf, errors) == -1)
        goto errorexit;

    if (doflush && iconvencoder_flush(self, ic, &buf, errors) == -1)
        goto errorexit;

    finalsize = (int)((char*)buf.outbuf - PyString_AS_STRING(buf.outobj));

    if (finalsize != PyString_GET_SIZE(buf.outobj))
        if (_PyString_Resize(&buf.outobj, finalsize) == -1)
            goto errorexit;

    Py_XDECREF(buf.excobj);
    if (buf.rinbuf_top != NULL)
        PyMem_Del((void *)buf.rinbuf_top);

    return buf.outobj;

errorexit:
    Py_XDECREF(buf.excobj);
    Py_XDECREF(buf.outobj);
    if (buf.rinbuf_top != NULL)
        PyMem_Del((void *)buf.rinbuf_top);

    return NULL;
}

static PyObject *
iconvencoder_call(IconvEncoderObject *self,
                  PyObject *args, PyObject *kwargs)
{
    Py_UNICODE      *data;
    PyObject        *errorcb, *r;
    iconv_t          ic;
    const char      *errors = NULL;
    int              datalen;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "u#|z:encode",
                            kwarglist, &data, &datalen, &errors))
        return NULL;

    errorcb = get_errorcallback(errors);
    if (errorcb == NULL)
        return NULL;

    ic = iconv_open(self->encoding, self->unicode_encoding);
    if (ic == (iconv_t)-1) {
        PyErr_SetString(PyExc_RuntimeError, "iconv_open failed");
        goto errorexit;
    }

    r = iconvencoder_encode(self, ic, data, datalen, errorcb, 1);
    iconv_close(ic);
    if (r == NULL)
        goto errorexit;

    if (errorcb > ERROR_MAX) {
        Py_DECREF(errorcb);
    }
    return make_tuple(r, datalen);

errorexit:
    if (errorcb > ERROR_MAX) {
        Py_DECREF(errorcb);
    }

    return NULL;
}

PyDoc_STRVAR(iconvencoder_makestream__doc__,
"I.makestream(stream, errors='strict')\n"
"Return an StreamWriter instance of `I' iconv encoder.");

static PyObject *
iconvencoder_makestream(IconvEncoderObject *self,
                        PyObject *args, PyObject *kwargs)
{
    static char *stream_kwarglist[] = {"stream", "errors", NULL};
    PyObject    *stream;
    char        *errors = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|s:makestream",
                            stream_kwarglist, &stream, &errors))
        return NULL;

    return iconvstreamwriter_create(self, stream, errors);
}

static struct PyMethodDef iconvencoder_methods[] = {
    {"makestream",  (PyCFunction)iconvencoder_makestream,
                    METH_VARARGS | METH_KEYWORDS,
                    iconvencoder_makestream__doc__},
    {NULL,          NULL},
};

OLD_GETATTR_DEF(iconvencoder)

static void
iconvencoder_dealloc(IconvEncoderObject *self)
{
    if (self->encoding != NULL)
        free(self->encoding);
    PyObject_Del(self);
}

#ifndef LACKS_PYSTRING_FROMFORMAT
static PyObject *
iconvencoder_repr(IconvEncoderObject *self)
{
    return PyString_FromFormat(
            "<IconvEncoder from='%s' to='%s' mode='%s'>",
            self->unicode_encoding, self->encoding,
            uniinternal_type_names[self->unitype]);
}
#endif

static PyTypeObject IconvEncoder_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "IconvEncoder",                 /*tp_name*/
    sizeof(IconvEncoderObject),     /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    /* methods */
    (destructor)iconvencoder_dealloc, /*tp_dealloc*/
    0,                              /*tp_print*/
    GETATTR_FUNC(iconvencoder),     /*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
#ifndef LACKS_PYSTRING_FROMFORMAT
    (reprfunc)iconvencoder_repr,    /*tp_repr*/
#else
    0,                              /*tp_repr*/
#endif
    0,                              /*tp_as_number*/
    0,                              /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash*/
    (ternaryfunc)iconvencoder_call, /*tp_call*/
    0,                              /*tp_str*/
    GETATTRO_FUNC,                  /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
    iconvencoder_doc,               /*tp_doc*/
#ifndef OLD_STYLE_TYPE
    0,                              /*tp_traverse*/
    0,                              /*tp_clear*/
    0,                              /*tp_richcompare*/
    0,                              /*tp_weaklistoffset*/
    0,                              /*tp_iter*/
    0,                              /*tp_iterext*/
    iconvencoder_methods,           /*tp_methods*/
#endif
};

static int
expand_decodebuffer(IconvDecoderBuffer *buf, int esize)
{
    int  orgpos, orgsize;

    orgpos  = (int)(buf->outbuf - PyUnicode_AS_UNICODE(buf->outobj));
    orgsize = PyUnicode_GET_SIZE(buf->outobj);
    if (PyUnicode_Resize(&buf->outobj, orgsize + (
                esize < (orgsize >> 1) ? (orgsize >> 1) | 1 : esize)) == -1)
        return -1;

    buf->outbuf = PyUnicode_AS_UNICODE(buf->outobj) + orgpos;
    buf->outbuf_end = PyUnicode_AS_UNICODE(buf->outobj)
                       + PyUnicode_GET_SIZE(buf->outobj);

    return 0;
}
#define RESERVE_DECODEBUFFER(buf, s) {                          \
    if ((s) < 1 || (buf)->outbuf + (s) > (buf)->outbuf_end)     \
        if (expand_decodebuffer(buf, s) == -1)                  \
            goto errorexit;                                     \
}

static int
iconvdecoder_error(IconvDecoderObject *self,
                   iconv_t ic, IconvDecoderBuffer *buf,
                   PyObject *errors, int eno, int esize)
{
#ifndef LACKS_ERROR_CALLBACKS
    PyObject    *argsobj, *retuni = NULL, *retobj = NULL;
    int          retunisize, newpos;
#endif
    const char  *reason;
    int          start, end;

    if (PyErr_Occurred())
        return -1; /* treat as an internal error */

    if (eno == E2BIG) {
        RESERVE_DECODEBUFFER(buf, -1);
        return 0; /* retry it */
    }

    switch (eno) {
#if __CYGWIN__
    case ENOENT: /* FALLTHROUGH */
#endif
    case EILSEQ:
        reason = "illegal multibyte sequence"; break;
    case EINVAL:
        reason = "incomplete multibyte sequence"; break;
    default:
        PyErr_SetString(PyExc_RuntimeError,
                        "unknown runtime error from iconv");
        return -1;
    }

    if (errors == ERROR_REPLACE) {
        RESERVE_DECODEBUFFER(buf, 1);
        *buf->outbuf++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        buf->inbuf += esize;
        return 0;
    } else if (errors == ERROR_IGNORE) {
        buf->inbuf += esize;
        return 0;
    }

    start = (int)(buf->inbuf - buf->inbuf_top);
    end = start + esize;

#ifdef LACKS_ERROR_CALLBACKS
    if (esize == 1)
        PyErr_Format(PyExc_UnicodeError,
            "'%s' codec can't decode byte 0x%02x in position %d: %s",
            self->encoding, *buf->inbuf, start, reason);
    else
        PyErr_Format(PyExc_UnicodeError,
            "'%s' codec can't decode bytes in position %d-%d: %s",
            self->encoding, start, end, reason);
errorexit:
    return -1;
#else
    /* use cached exception object if available */
    if (buf->excobj == NULL) {
        buf->excobj = PyUnicodeDecodeError_Create(
                self->encoding, buf->inbuf_top,
                (size_t)(buf->inbuf_end - buf->inbuf_top), start, end, reason);
        if (buf->excobj == NULL)
            goto errorexit;
    } else
        if (PyUnicodeDecodeError_SetStart(buf->excobj, start) ||
            PyUnicodeDecodeError_SetEnd(buf->excobj, end) ||
            PyUnicodeDecodeError_SetReason(buf->excobj, reason))
                goto errorexit;

    if (errors == ERROR_STRICT) {
        PyCodec_StrictErrors(buf->excobj);
        goto errorexit;
    }

    argsobj = PyTuple_New(1);
    if (argsobj == NULL)
        goto errorexit;

    PyTuple_SET_ITEM(argsobj, 0, buf->excobj);
    Py_INCREF(buf->excobj);
    retobj = PyObject_CallObject(errors, argsobj);
    Py_DECREF(argsobj);
    if (retobj == NULL)
        goto errorexit;

    if (!PyTuple_Check(retobj) || PyTuple_GET_SIZE(retobj) != 2 ||
        !PyUnicode_Check((retuni = PyTuple_GET_ITEM(retobj, 0))) ||
        !PyInt_Check(PyTuple_GET_ITEM(retobj, 1))) {
        PyErr_SetString(PyExc_ValueError,
            "decoding error handler must return (unicode, int) tuple");
        goto errorexit;
    }

    retunisize = PyUnicode_GET_SIZE(retuni);
    if (retunisize > 0) {
        RESERVE_DECODEBUFFER(buf, retunisize);
        memcpy((char *)buf->outbuf, PyUnicode_AS_DATA(retuni),
                retunisize * Py_UNICODE_SIZE);
        buf->outbuf += retunisize;
    }

    newpos = (int)PyInt_AS_LONG(PyTuple_GET_ITEM(retobj, 1));
    if (newpos < 0)
        newpos += (int)(buf->inbuf_end - buf->inbuf_top);
    if (newpos < 0 || buf->inbuf_top + newpos > buf->inbuf_end) {
        PyErr_Format(PyExc_IndexError,
            "position %d from error handler out of bounds", newpos);
        goto errorexit;
    }
    buf->inbuf = buf->inbuf_top + newpos;
    Py_DECREF(retobj);
    return 0;

errorexit:
    Py_XDECREF(retobj);
    return -1;
#endif
}

static size_t
iconvwrap_ucsswapped(iconv_t ic,
                     iconv_arg2_t inbuf, size_t *inleft,
                     char **outbuf, size_t *outleft)
{
    Py_UNICODE  *outbuf_s;
    size_t       r;

    outbuf_s = (Py_UNICODE *)*outbuf;
    r = iconv(ic, inbuf, inleft, outbuf, outleft);

    for (;outbuf_s < (Py_UNICODE *)*outbuf; outbuf_s++)
        *outbuf_s = _Py_UNICODE_SWAP(*outbuf_s);

    return r;
}

static size_t
iconvwrap_utf8(iconv_t ic, IconvDecoderBuffer *buf,
               size_t inleft, size_t outleft)
{
    unsigned char   *ubuf, *ubuf_top, *ubuf_end;
    size_t           r;
    int              nch;

    ubuf = PyMem_Malloc(outleft * MBENCODED_LENGTH_MAX / Py_UNICODE_SIZE);
    if (ubuf == NULL)
        return -1;
    ubuf_top = ubuf;

    if (inleft > 0)
        r = iconv(ic, (iconv_arg2_t)&(buf->inbuf), &inleft,
                      (char **)&ubuf, &outleft);
    else
        r = iconv(ic, NULL, NULL, (char **)&ubuf, &outleft);

    ubuf_end = ubuf;
    nch = countchars_utf8(ubuf_top, ubuf_end);
    if (nch > 0)
        RESERVE_DECODEBUFFER(buf, nch)
    for (ubuf = ubuf_top; ubuf < ubuf_end;) {
        int      uleft = (int)(ubuf_end - ubuf);
        ucs4_t   code;

        if (*ubuf < 0x80) {
            code = (unsigned char)*ubuf++;
        } else if (*ubuf < 0xc2) {
ilseq:      PyErr_SetString(PyExc_RuntimeError,
                            "iconv returned illegal utf-8 sequence");
            goto errorexit;
        } else if (*ubuf < 0xe0) {
            if (uleft < 2 || !((ubuf[1] ^ 0x80) < 0x40))
                goto ilseq;
            code = ((ucs4_t)(ubuf[0] & 0x1f) << 6)
                   | (ucs4_t)(ubuf[1] ^ 0x80);
            ubuf += 2;
        } else if (*ubuf < 0xf0) {
            if (uleft < 3 || !((ubuf[1] ^ 0x80) < 0x40 &&
                (ubuf[2] ^ 0x80) < 0x40 &&
                (ubuf[0] >= 0xe1 || ubuf[1] >= 0xa0)))
                goto ilseq;
            code = ((ucs4_t)(ubuf[0] & 0x0f) << 12)
                   | ((ucs4_t)(ubuf[1] ^ 0x80) << 6)
                   | (ucs4_t)(ubuf[2] ^ 0x80);
            ubuf += 3;
        }
        else if (*ubuf < 0xf8) {
            if (uleft < 4 || !((ubuf[1] ^ 0x80) < 0x40 &&
                (ubuf[2] ^ 0x80) < 0x40 && (ubuf[3] ^ 0x80) < 0x40 &&
                (ubuf[0] >= 0xf1 || ubuf[1] >= 0x90)))
                goto ilseq;
            code = ((ucs4_t)(ubuf[0] & 0x07) << 18)
                   | ((ucs4_t)(ubuf[1] ^ 0x80) << 12)
                   | ((ucs4_t)(ubuf[2] ^ 0x80) << 6)
                   | (ucs4_t)(ubuf[3] ^ 0x80);
            ubuf += 4;
        } else if (*ubuf < 0xfc) {
            if (uleft < 5 || !((ubuf[1] ^ 0x80) < 0x40 &&
                (ubuf[2] ^ 0x80) < 0x40 && (ubuf[3] ^ 0x80) < 0x40 &&
                (ubuf[4] ^ 0x80) < 0x40 && (ubuf[0] >= 0xf9 ||
                ubuf[1] >= 0x88)))
                goto ilseq;
            code = ((ucs4_t)(ubuf[0] & 0x03) << 24)
                   | ((ucs4_t)(ubuf[1] ^ 0x80) << 18)
                   | ((ucs4_t)(ubuf[2] ^ 0x80) << 12)
                   | ((ucs4_t)(ubuf[3] ^ 0x80) << 6)
                   | (ucs4_t)(ubuf[4] ^ 0x80);
            ubuf += 5;
        } else if (*ubuf < 0xfe) {
            if (uleft < 6 || !((ubuf[1] ^ 0x80) < 0x40 &&
                (ubuf[2] ^ 0x80) < 0x40 && (ubuf[3] ^ 0x80) < 0x40 &&
                (ubuf[4] ^ 0x80) < 0x40 && (ubuf[5] ^ 0x80) < 0x40 &&
                (ubuf[0] >= 0xfd || ubuf[1] >= 0x84)))
                goto ilseq;
            code = ((ucs4_t)(ubuf[0] & 0x01) << 30)
                   | ((ucs4_t)(ubuf[1] ^ 0x80) << 24)
                   | ((ucs4_t)(ubuf[2] ^ 0x80) << 18)
                   | ((ucs4_t)(ubuf[3] ^ 0x80) << 12)
                   | ((ucs4_t)(ubuf[4] ^ 0x80) << 6)
                   | (ucs4_t)(ubuf[5] ^ 0x80);
            ubuf += 6;
        } else 
            goto ilseq;

#if Py_UNICODE_SIZE == 2
        if (code >= 0x10000) {
            if (code >= 0x110000)
                goto ilseq;

            if (buf->outbuf_end <= buf->outbuf + 1) {
                RESERVE_DECODEBUFFER(buf, -1)
            }
            *buf->outbuf++ = 0xd800 + ((code - 0x10000) >> 10);
            *buf->outbuf++ = 0xdc00 + ((code - 0x10000) & 0x3ff);
        } else 
#endif
        {
            if (buf->outbuf_end <= buf->outbuf) {
                RESERVE_DECODEBUFFER(buf, -1)
            }
            *buf->outbuf++ = (Py_UNICODE)code;
        }
    }

    PyMem_Del(ubuf_top);
    return r;

errorexit:
    PyMem_Del(ubuf);
    return -1;
}

static int
iconvdecoder_conv(IconvDecoderObject *self, iconv_t ic,
                  IconvDecoderBuffer *buf, PyObject *errors)
{
    size_t   r, inleft, outleft;

    for (;;) {
        inleft = (size_t)(buf->inbuf_end - buf->inbuf);
        if (inleft == 0)
            return 0;
        outleft = (size_t)buf->outbuf_end - (size_t)buf->outbuf;

        if (self->iconvwrap != NULL)
            r = self->iconvwrap(ic, (iconv_arg2_t)&(buf->inbuf), &inleft,
                                    (char **)&(buf->outbuf), &outleft);
        else
            r = self->iconvwrap2(ic, buf, inleft, outleft);

        if (r == (size_t)-1) {
            if (errno == EINVAL) /* leave the incomplete sequence */
                return 0;
            else if (iconvdecoder_error(self, ic, buf, errors, errno, 1))
                return -1;
            else
                continue;
        } else
            break;
    }

    return 0;
}

static int
iconvdecoder_flush(IconvDecoderObject *self,
                   iconv_t ic, IconvDecoderBuffer *buf,
                   PyObject *errors)
{
    size_t   r, outleft;

    if (buf->inbuf < buf->inbuf_end) /* assumes as left by EINVAL */
        if (iconvdecoder_error(self, ic, buf, errors, EINVAL,
                (int)(buf->inbuf_end - buf->inbuf)))
            goto errorexit;

    for (;;) {
        outleft = (size_t)buf->outbuf_end - (size_t)buf->outbuf;

        if (self->iconvwrap != NULL)
            r = self->iconvwrap(ic, NULL, NULL,
                                    (char **)&(buf->outbuf), &outleft);
        else
            r = self->iconvwrap2(ic, buf, 0, outleft);

        if (r == (size_t)-1) {
            if (errno == E2BIG) {
                RESERVE_DECODEBUFFER(buf, -1);
                continue;
            } else {
                if (iconvdecoder_error(self, ic, buf, errors, errno, 0))
                    goto errorexit;
            }
        }
        break;
    }

    return 0;

errorexit:
    return -1;
}

static PyObject *
iconvdecoder_call(IconvDecoderObject *self,
                  PyObject *args, PyObject *kwargs)
{
    IconvDecoderBuffer   buf;
    PyObject    *errorcb;
    iconv_t      ic;
    const char  *data, *errors = NULL;
    int          datalen, finalsize;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|z:decode",
                            kwarglist, &data, &datalen, &errors))
        return NULL;

    errorcb = get_errorcallback(errors);
    if (errorcb == NULL)
        return NULL;

    if (datalen == 0) {
        if (errorcb > ERROR_MAX)
            {Py_DECREF(errorcb);}
        return make_tuple(PyUnicode_FromUnicode(NULL, 0), 0);
    }

    buf.outobj = buf.excobj = NULL;
    ic = iconv_open(self->unicode_encoding, self->encoding);
    if (ic == (iconv_t)-1) {
        PyErr_SetString(PyExc_RuntimeError, "iconv_open failed");
        goto errorexit_noclose;
    }

    buf.inbuf = buf.inbuf_top = (unsigned char *)data;
    buf.inbuf_end = buf.inbuf_top + datalen;
    buf.outobj = PyUnicode_FromUnicode(NULL, datalen);
    if (buf.outobj == NULL)
        goto errorexit;
    buf.outbuf = PyUnicode_AS_UNICODE(buf.outobj);
    buf.outbuf_end = buf.outbuf + PyUnicode_GET_SIZE(buf.outobj);

    if (iconvdecoder_conv(self, ic, &buf, errorcb) ||
        iconvdecoder_flush(self, ic, &buf, errorcb))
            goto errorexit;

    finalsize = (int)(buf.outbuf - PyUnicode_AS_UNICODE(buf.outobj));

    if (finalsize != PyUnicode_GET_SIZE(buf.outobj))
        if (PyUnicode_Resize(&buf.outobj, finalsize) == -1)
            goto errorexit;

    Py_XDECREF(buf.excobj);
    iconv_close(ic);

    if (errorcb > ERROR_MAX)
        {Py_DECREF(errorcb);}
    return make_tuple(buf.outobj, datalen);

errorexit:
    iconv_close(ic);
errorexit_noclose:
    if (errorcb > ERROR_MAX)
        {Py_DECREF(errorcb);}
    Py_XDECREF(buf.excobj);
    Py_XDECREF(buf.outobj);

    return NULL;
}

PyDoc_STRVAR(iconvdecoder_makestream__doc__,
"I.makestream(stream, errors='strict')\n"
"Return an StreamReader instance of `I' iconv decoder.");

static PyObject *
iconvdecoder_makestream(IconvDecoderObject *self,
                        PyObject *args, PyObject *kwargs)
{
    static char *stream_kwarglist[] = {"stream", "errors", NULL};
    PyObject    *stream;
    char        *errors = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|s:makestream",
                            stream_kwarglist, &stream, &errors))
        return NULL;

    return iconvstreamreader_create(self, stream, errors);
}

static struct PyMethodDef iconvdecoder_methods[] = {
    {"makestream",  (PyCFunction)iconvdecoder_makestream,
                    METH_VARARGS | METH_KEYWORDS,
                    iconvdecoder_makestream__doc__},
    {NULL,          NULL},
};

OLD_GETATTR_DEF(iconvdecoder)

static void
iconvdecoder_dealloc(IconvDecoderObject *self)
{
    if (self->encoding != NULL)
        free(self->encoding);
    PyObject_Del(self);
}

#ifndef LACKS_PYSTRING_FROMFORMAT
static PyObject *
iconvdecoder_repr(IconvDecoderObject *self)
{
    return PyString_FromFormat(
            "<IconvDecoder from='%s' to='%s' mode='%s'>",
            self->encoding, self->unicode_encoding,
            uniinternal_type_names[self->unitype]);
}
#endif

static PyTypeObject IconvDecoder_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "IconvDecoder",                 /*tp_name*/
    sizeof(IconvDecoderObject),     /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    /* methods */
    (destructor)iconvdecoder_dealloc, /*tp_dealloc*/
    0,                              /*tp_print*/
    GETATTR_FUNC(iconvdecoder),     /*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
#ifndef LACKS_PYSTRING_FROMFORMAT
    (reprfunc)iconvdecoder_repr,    /*tp_repr*/
#else
    0,                              /*tp_repr*/
#endif
    0,                              /*tp_as_number*/
    0,                              /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash*/
    (ternaryfunc)iconvdecoder_call, /*tp_call*/
    0,                              /*tp_str*/
    GETATTRO_FUNC,                  /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
    iconvdecoder_doc,               /*tp_doc*/
#ifndef OLD_STYLE_TYPE
    0,                              /*tp_traverse*/
    0,                              /*tp_clear*/
    0,                              /*tp_richcompare*/
    0,                              /*tp_weaklistoffset*/
    0,                              /*tp_iter*/
    0,                              /*tp_iterext*/
    iconvdecoder_methods,           /*tp_methods*/
#endif
};

static PyObject *
iconvstreamreader_iread(IconvStreamReaderObject *self,
                        const char *method, int sizehint)
{
    IconvDecoderBuffer   buf;
    PyObject    *cres;
    int          rsize, r, finalsize = 0;

    if (sizehint == 0)
        return PyUnicode_FromUnicode(NULL, 0);

    buf.outobj = buf.excobj = NULL;
    cres = NULL;

    for (;;) {
        if (sizehint < 0)
            cres = PyObject_CallMethod(self->stream, (char *)method, NULL);
        else
            cres = PyObject_CallMethod(self->stream,
                            (char *)method, "i", sizehint);
        if (cres == NULL)
            goto errorexit;

        if (!PyString_Check(cres)) {
            PyErr_SetString(PyExc_TypeError,
                            "stream function returned a non-string object");
            goto errorexit;
        }

        if (self->pendingsize > 0) {
            PyObject    *ctr;
            char        *ctrdata;

            rsize = PyString_GET_SIZE(cres) + self->pendingsize;
            ctr = PyString_FromStringAndSize(NULL, rsize);
            if (ctr == NULL)
                goto errorexit;
            ctrdata = PyString_AS_STRING(ctr);
            memcpy(ctrdata, self->pending, self->pendingsize);
            memcpy(ctrdata + self->pendingsize,
                   PyString_AS_STRING(cres), PyString_GET_SIZE(cres));
            Py_DECREF(cres);
            cres = ctr;
            self->pendingsize = 0;
        }

        rsize = PyString_GET_SIZE(cres);
        buf.inbuf = buf.inbuf_top = (unsigned char *)PyString_AS_STRING(cres);
        buf.inbuf_end = buf.inbuf_top + rsize;
        if (buf.outobj == NULL) {
            buf.outobj = PyUnicode_FromUnicode(NULL, rsize);
            if (buf.outobj == NULL)
                goto errorexit;
            buf.outbuf = PyUnicode_AS_UNICODE(buf.outobj);
            buf.outbuf_end = buf.outbuf + PyUnicode_GET_SIZE(buf.outobj);
        }

        if (rsize > 0)
            r = iconvdecoder_conv(self->codec,
                                  self->ic, &buf, self->errors);
        else
            r = 0;

        if (rsize == 0 || (r == 0 && sizehint < 0)) /* end of file */
            r = iconvdecoder_flush(self->codec,
                                   self->ic, &buf, self->errors);

        if (r)
            goto errorexit;

        if (buf.inbuf < buf.inbuf_end) { /* pending sequence exists */
            size_t       npendings;

            /* we can't assume that pendingsize is still 0 here. because
             * this function can be called recursively from error callback */
            npendings = (size_t)(buf.inbuf_end - buf.inbuf);
            if (npendings + self->pendingsize > MAXPENDING) {
                PyErr_SetString(PyExc_RuntimeError,
                                "pending buffer overflow");
                goto errorexit;
            }
            memcpy(self->pending + self->pendingsize, buf.inbuf, npendings);
            self->pendingsize += npendings;
        }

        finalsize = (int)(buf.outbuf - PyUnicode_AS_UNICODE(buf.outobj));

        Py_DECREF(cres);
        cres = NULL;

        if (sizehint < 0 || finalsize != 0 || rsize == 0)
            break;

        sizehint = 1; /* read 1 more byte and retry */
    }

    if (finalsize != PyUnicode_GET_SIZE(buf.outobj))
        if (PyUnicode_Resize(&buf.outobj, finalsize) == -1)
            goto errorexit;

    Py_XDECREF(cres);
    Py_XDECREF(buf.excobj);
    return buf.outobj;

errorexit:
    Py_XDECREF(cres);
    Py_XDECREF(buf.excobj);
    Py_XDECREF(buf.outobj);
    return NULL;
}

static PyObject *
iconvstreamreader_read(IconvStreamReaderObject *self, PyObject *args)
{
    PyObject    *sizeobj = NULL;
    long         size;

    if (!PyArg_ParseTuple(args, "|O:read", &sizeobj))
        return NULL;

    if (sizeobj == Py_None || sizeobj == NULL)
        size = -1;
    else if (PyInt_Check(sizeobj))
        size = PyInt_AsLong(sizeobj);
    else {
        PyErr_SetString(PyExc_TypeError, "arg 1 must be an integer");
        return NULL;
    }

    return iconvstreamreader_iread(self, "read", size);
}

static PyObject *
iconvstreamreader_readline(IconvStreamReaderObject *self, PyObject *args)
{
    PyObject    *sizeobj = NULL;
    long         size;

    if (!PyArg_ParseTuple(args, "|O:readline", &sizeobj))
        return NULL;

    if (sizeobj == Py_None || sizeobj == NULL)
        size = -1;
    else if (PyInt_Check(sizeobj))
        size = PyInt_AsLong(sizeobj);
    else {
        PyErr_SetString(PyExc_TypeError, "arg 1 must be an integer");
        return NULL;
    }

    return iconvstreamreader_iread(self, "readline", size);
}

static PyObject *
iconvstreamreader_readlines(IconvStreamReaderObject *self, PyObject *args)
{
    PyObject    *sizehintobj = NULL, *r, *sr;
    long         sizehint;

    if (!PyArg_ParseTuple(args, "|O:readlines", &sizehintobj))
        return NULL;

    if (sizehintobj == Py_None || sizehintobj == NULL)
        sizehint = -1;
    else if (PyInt_Check(sizehintobj))
        sizehint = PyInt_AsLong(sizehintobj);
    else {
        PyErr_SetString(PyExc_TypeError, "arg 1 must be an integer");
        return NULL;
    }

    r = iconvstreamreader_iread(self, "read", sizehint);
    if (r == NULL)
        return NULL;

    sr = PyUnicode_Splitlines(r, 1);
    Py_DECREF(r);
    return sr;
}

static PyObject *
iconvstreamreader_reset(IconvStreamReaderObject *self)
{
    if (iconv(self->ic, NULL, NULL, NULL, NULL) == (size_t)-1) {
        PyErr_SetString(PyExc_RuntimeError, "iconv reset failed");
        return NULL;
    }
    self->pendingsize = 0;

    Py_INCREF(Py_None);
    return Py_None;
}

static struct PyMethodDef iconvstreamreader_methods[] = {
    {"read",        (PyCFunction)iconvstreamreader_read,
                    METH_VARARGS, NULL},
    {"readline",    (PyCFunction)iconvstreamreader_readline,
                    METH_VARARGS, NULL},
    {"readlines",   (PyCFunction)iconvstreamreader_readlines,
                    METH_VARARGS, NULL},
    {"reset",       (PyCFunction)iconvstreamreader_reset,
                    METH_NOARGS, NULL},
    {NULL,          NULL},
};

OLD_GETATTR_DEF(iconvstreamreader)

static void
iconvstreamreader_dealloc(IconvStreamReaderObject *self)
{
    if (self->stream != NULL) { /* is initialized? */
        Py_DECREF(self->stream);
        Py_DECREF(self->codec);
        iconv_close(self->ic);
    }
    if (self->errors > ERROR_MAX) {
        Py_DECREF(self->errors);
    }
    PyObject_Del(self);
}

#ifndef LACKS_PYSTRING_FROMFORMAT
static PyObject *
iconvstreamreader_repr(IconvStreamReaderObject *self)
{
    return PyString_FromFormat(
            "<IconvStreamReader from='%s' to='%s' mode='%s'>",
            self->codec->encoding, self->codec->unicode_encoding,
            uniinternal_type_names[self->codec->unitype]);
}
#endif

static PyTypeObject IconvStreamReader_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "IconvStreamReader",            /*tp_name*/
    sizeof(IconvStreamReaderObject), /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    /* methods */
    (destructor)iconvstreamreader_dealloc, /*tp_dealloc*/
    0,                              /*tp_print*/
    GETATTR_FUNC(iconvstreamreader),/*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
#ifndef LACKS_PYSTRING_FROMFORMAT
    (reprfunc)iconvstreamreader_repr,  /*tp_repr*/
#else
    0,                              /*tp_repr*/
#endif
    0,                              /*tp_as_nuiconver*/
    0,                              /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash*/
    0,                              /*tp_call*/
    0,                              /*tp_str*/
    GETATTRO_FUNC,                  /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
#ifndef OLD_STYLE_TYPE
    0,                              /*tp_doc*/
    0,                              /*tp_traverse*/
    0,                              /*tp_clear*/
    0,                              /*tp_richcompare*/
    0,                              /*tp_weaklistoffset*/
    0,                              /*tp_iter*/
    0,                              /*tp_iterext*/
    iconvstreamreader_methods,      /*tp_methods*/
#endif
};

static int
iconvstreamwriter_iwrite(IconvStreamWriterObject *self,
                         PyObject *unistr)
{
    IconvEncoderBuffer   buf;
    PyObject    *wr;
    int          rsize, r, finalsize = 0;

    if (!PyUnicode_Check(unistr)) {
        PyErr_SetString(PyExc_TypeError,
                        "only unicode objects are encodable.");
        return -1;
    }

    rsize = PyUnicode_GET_SIZE(unistr);
    if (iconvencoder_prepbuf(&buf, (Py_UNICODE *)PyUnicode_AS_UNICODE(unistr),
                rsize, self->codec->unitype) == -1)
        goto errorexit;

    if (rsize == 0) /* zero buffer flushes the internal context */
        r = iconvencoder_flush(self->codec, self->ic, &buf, self->errors);
    else
        r = iconvencoder_conv(self->codec, self->ic, &buf, self->errors);
    if (r != 0)
        goto errorexit;

    finalsize = (int)(buf.outbuf -
                      (unsigned char *)PyString_AS_STRING(buf.outobj));
    if (finalsize != PyString_GET_SIZE(buf.outobj))
        if (_PyString_Resize(&buf.outobj, finalsize) == -1)
            goto errorexit;

    wr = PyObject_CallMethod(self->stream, "write", "O", buf.outobj);
    if (wr == NULL)
        goto errorexit;

    if (buf.rinbuf_top != NULL)
        PyMem_Free((void *)buf.rinbuf_top);
    Py_DECREF(buf.outobj);
    Py_XDECREF(buf.excobj);
    Py_DECREF(wr);
    return 0;

errorexit:
    if (buf.rinbuf_top != NULL)
        PyMem_Free((void *)buf.rinbuf_top);
    Py_XDECREF(buf.outobj);
    Py_XDECREF(buf.excobj);
    return -1;
}

static PyObject *
iconvstreamwriter_write(IconvStreamWriterObject *self, PyObject *args)
{
    PyObject    *strobj;

    if (!PyArg_ParseTuple(args, "O:write", &strobj))
        return NULL;

    if (iconvstreamwriter_iwrite(self, strobj))
        return NULL;
    else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}

static PyObject *
iconvstreamwriter_writelines(IconvStreamWriterObject *self, PyObject *args)
{
    PyObject    *lines, *strobj;
    int          i, r;

    if (!PyArg_ParseTuple(args, "O:writelines", &lines))
        return NULL;

    if (!PySequence_Check(lines)) {
        PyErr_SetString(PyExc_TypeError, "arg must be a sequence object");
        return NULL;
    }

    for (i = 0; i < PySequence_Length(lines); i++) {
        /* length can be changed even within this loop */
        strobj = PySequence_GetItem(lines, i);
        if (strobj == NULL)
            return NULL;

        r = iconvstreamwriter_iwrite(self, strobj);
        Py_DECREF(strobj);
        if (r == -1)
            return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
iconvstreamwriter_reset(IconvStreamWriterObject *self)
{
    if (iconv(self->ic, NULL, NULL, NULL, NULL) == (size_t)-1) {
        PyErr_SetString(PyExc_RuntimeError, "iconv reset failed");
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static struct PyMethodDef iconvstreamwriter_methods[] = {
    {"write",       (PyCFunction)iconvstreamwriter_write,
                    METH_VARARGS, NULL},
    {"writelines",  (PyCFunction)iconvstreamwriter_writelines,
                    METH_VARARGS, NULL},
    {"reset",       (PyCFunction)iconvstreamwriter_reset,
                    METH_NOARGS, NULL},
    {NULL,          NULL},
};

OLD_GETATTR_DEF(iconvstreamwriter)

static void
iconvstreamwriter_dealloc(IconvStreamWriterObject *self)
{
    if (self->stream != NULL) { /* is initialized? */
        Py_DECREF(self->stream);
        Py_DECREF(self->codec);
        iconv_close(self->ic);
    }
    if (self->errors > ERROR_MAX) {
        Py_DECREF(self->errors);
    }
    PyObject_Del(self);
}

#ifndef LACKS_PYSTRING_FROMFORMAT
static PyObject *
iconvstreamwriter_repr(IconvStreamWriterObject *self)
{
    return PyString_FromFormat(
            "<IconvStreamWriter from='%s' to='%s' mode='%s'>",
            self->codec->unicode_encoding, self->codec->encoding,
            uniinternal_type_names[self->codec->unitype]);
}
#endif

static PyTypeObject IconvStreamWriter_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "IconvStreamWriter",            /*tp_name*/
    sizeof(IconvStreamWriterObject), /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    /* methods */
    (destructor)iconvstreamwriter_dealloc, /*tp_dealloc*/
    0,                              /*tp_print*/
    GETATTR_FUNC(iconvstreamwriter),/*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
#ifndef LACKS_PYSTRING_FROMFORMAT
    (reprfunc)iconvstreamwriter_repr,  /*tp_repr*/
#else
    0,                              /*tp_repr*/
#endif
    0,                              /*tp_as_nuiconver*/
    0,                              /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash*/
    0,                              /*tp_call*/
    0,                              /*tp_str*/
    GETATTRO_FUNC,                  /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
#ifndef OLD_STYLE_TYPE
    0,                              /*tp_doc*/
    0,                              /*tp_traverse*/
    0,                              /*tp_clear*/
    0,                              /*tp_richcompare*/
    0,                              /*tp_weaklistoffset*/
    0,                              /*tp_iter*/
    0,                              /*tp_iterext*/
    iconvstreamwriter_methods,      /*tp_methods*/
#endif
};

static PyObject *
iconvstreamreader_create(IconvDecoderObject *decoder,
                         PyObject *stream, const char *errors)
{
    IconvStreamReaderObject *self;

    self = PyObject_New(IconvStreamReaderObject, &IconvStreamReader_Type);
    if (self == NULL)
        return NULL;

    self->errors = get_errorcallback(errors);
    if (self->errors == NULL) {
        self->codec = NULL;
        self->stream = NULL;
        Py_DECREF(self);
        return NULL;
    }
    self->codec = decoder;
    Py_INCREF(decoder);
    self->stream = stream;
    Py_INCREF(stream);
    self->pendingsize = 0;

    self->ic = iconv_open(decoder->unicode_encoding, decoder->encoding);
    if (self->ic == (iconv_t)-1) {
        PyErr_SetString(PyExc_RuntimeError, "iconv_open failed");
        Py_DECREF(self);
        return NULL;
    }

    return (PyObject *)self;
}

static PyObject *
iconvstreamwriter_create(IconvEncoderObject *encoder,
                         PyObject *stream, const char *errors)
{
    IconvStreamWriterObject *self;

    self = PyObject_New(IconvStreamWriterObject, &IconvStreamWriter_Type);
    if (self == NULL)
        return NULL;

    self->errors = get_errorcallback(errors);
    if (self->errors == NULL) {
        self->codec = NULL;
        self->stream = NULL;
        Py_DECREF(self);
        return NULL;
    }
    self->codec = encoder;
    Py_INCREF(encoder);
    self->stream = stream;
    Py_INCREF(stream);

    self->ic = iconv_open(encoder->encoding, encoder->unicode_encoding);
    if (self->ic == (iconv_t)-1) {
        PyErr_SetString(PyExc_RuntimeError, "iconv_open failed");
        Py_DECREF(self);
        return NULL;
    }

    return (PyObject *)self;
}

PyDoc_STRVAR(iconvcodec_makeencoder__doc__,
"makeencoder(encoding): open an iconv(3) encoder instance");

static PyObject *
iconvcodec_makeencoder(PyObject *spam, PyObject *args)
{
    IconvEncoderObject  *self;
    iconv_t  ic;
    char    *encoding;
    int      i;

    if (!PyArg_ParseTuple(args, "s:makeencoder", &encoding))
        return NULL;

    self = PyObject_New(IconvEncoderObject, &IconvEncoder_Type);
    if (self == NULL)
        return NULL;

#define um uniinternal_modes
    for (i = 0; um[i].encoding[0]; i++) {
        if (um[i].type == UNIINTERNAL_DONTUSE)
            continue;

        ic = iconv_open(encoding, um[i].encoding);
        if (ic == (iconv_t)-1)
            continue;
        else {
            iconv_close(ic);

            self->encoding = strdup(encoding);
            if (self->encoding == NULL) {
                PyErr_NoMemory();
                Py_DECREF(self);
                return NULL;
            }
            self->unicode_encoding = um[i].encoding;
            self->unitype = um[i].type;

            return (PyObject *)self;
        }
    }
#undef um

    self->encoding = NULL;
    Py_DECREF(self);
    PyErr_Format(PyExc_LookupError, "encoding '%s' is not available",
                 encoding);
    return NULL;
}

PyDoc_STRVAR(iconvcodec_makedecoder__doc__,
"makedecoder(encoding): open an iconv(3) decoder instance");

static PyObject *
iconvcodec_makedecoder(PyObject *spam, PyObject *args)
{
    IconvDecoderObject  *self;
    iconv_t  ic;
    char    *encoding;
    int      i;

    if (!PyArg_ParseTuple(args, "s:makedecoder", &encoding))
        return NULL;

    self = PyObject_New(IconvDecoderObject, &IconvDecoder_Type);
    if (self == NULL)
        return NULL;

#define um uniinternal_modes
    for (i = 0; um[i].encoding[0]; i++) {
        if (um[i].type == UNIINTERNAL_DONTUSE)
            continue;

        ic = iconv_open(um[i].encoding, encoding);
        if (ic == (iconv_t)-1)
            continue;
        else {
            iconv_close(ic);

            self->encoding = strdup(encoding);
            if (self->encoding == NULL) {
                PyErr_NoMemory();
                Py_DECREF(self);
                return NULL;
            }
            self->unicode_encoding = um[i].encoding;
            self->unitype = um[i].type;
            switch (self->unitype) {
            case UNIINTERNAL_UCS:
                self->iconvwrap = iconv;
                break;
            case UNIINTERNAL_UCS_SWAPPED:
                self->iconvwrap = iconvwrap_ucsswapped;
                break;
            case UNIINTERNAL_UTF_8:
                self->iconvwrap = NULL;
                self->iconvwrap2= iconvwrap_utf8;
                break;
            default:
                LOGICERROR();
                Py_DECREF(self);
                return NULL;
            }

            return (PyObject *)self;
        }
    }
#undef um

    self->encoding = NULL;
    Py_DECREF(self);
    PyErr_Format(PyExc_LookupError, "encoding '%s' is not available",
                 encoding);
    return NULL;
}

static struct PyMethodDef _iconv_codec_methods[] = {
    {"makeencoder", (PyCFunction)iconvcodec_makeencoder,
                    METH_VARARGS, iconvcodec_makeencoder__doc__},
    {"makedecoder", (PyCFunction)iconvcodec_makedecoder,
                    METH_VARARGS, iconvcodec_makedecoder__doc__},
    {NULL, NULL},
};

/* we test endians in runtime because condition can be changed even after
 * compilation by LD_PRELOAD or installing optional packages. */
static void
detect_iconv_endian(void)
{
    iconv_t  ic;
    int      i;

#define um uniinternal_modes
    for (i = 0; um[i].encoding[0]; i++) {
        const Py_UNICODE tchnative = '0';
        const char       tchin = '0', *tchin_buf = (const char *)&tchin;
        Py_UNICODE       tchout = 0;
        char            *tchout_buf = (char *)&tchout;
        size_t           tchin_len = 1, tchout_len = Py_UNICODE_SIZE;
        size_t           r, produced;

        if (um[i].type != UNIINTERNAL_UCS)
            continue; /* not UCS */

        ic = iconv_open(um[i].encoding, "ISO8859-1");
        if (ic == (iconv_t)-1) {
            ic = iconv_open(um[i].encoding, "ISO-8859-1");
            if (ic == (iconv_t)-1) {
                um[i].type = UNIINTERNAL_DONTUSE;
                continue; /* just failed */
            }
        }

        r = iconv(ic, (iconv_arg2_t)&tchin_buf, &tchin_len,
                      &tchout_buf, &tchout_len);
        iconv_close(ic);

        produced = (size_t)(tchout_buf - (char *)&tchout);
        if (r != (size_t)-1 && produced == Py_UNICODE_SIZE) {
            if (tchout == tchnative)
                continue; /* ucs native */
            else if (tchout == _Py_UNICODE_SWAP(tchnative)) {
                um[i].type = UNIINTERNAL_UCS_SWAPPED;
                continue; /* ucs swapped */
            }
        }

        um[i].type = UNIINTERNAL_DONTUSE;
        /* insane ucs support */
    }
#undef um
}

void
init_iconv_codec(void)
{
    detect_iconv_endian();

    Py_InitModule("_iconv_codec", _iconv_codec_methods);

    if (PyErr_Occurred())
        Py_FatalError("can't initialize the _iconv_codec module");
}

/*
 * ex: ts=8 sts=4 et
 */
