/*
 * codecentry.h: Common Codec Entry Routines
 *
 * Copyright (C) 2003 Hye-Shik Chang <perky@FreeBSD.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: codecentry.h,v 1.1 2003/09/24 17:44:51 perky Exp $
 */

#ifdef HAVE_ENCODER_INIT
#define ENCODER_INIT_FUNC(encoding)     encoding##_encode_init
#else
#define ENCODER_INIT_FUNC(encoding)     NULL
#endif

#ifdef HAVE_ENCODER_RESET
#define ENCODER_RESET_FUNC(encoding)    encoding##_encode_reset
#else
#define ENCODER_RESET_FUNC(encoding)    NULL
#endif

#ifdef HAVE_DECODER_INIT
#define DECODER_INIT_FUNC(encoding)     encoding##_decode_init
#else
#define DECODER_INIT_FUNC(encoding)     NULL
#endif

#ifdef HAVE_DECODER_RESET
#define DECODER_RESET_FUNC(encoding)    encoding##_decode_reset
#else
#define DECODER_RESET_FUNC(encoding)    NULL
#endif

#define BEGIN_CODEC_REGISTRY(encoding)                      \
    static MultibyteCodec __codec = {                       \
        #encoding,                                          \
        encoding##_encode,                                  \
        ENCODER_INIT_FUNC(encoding),                        \
        ENCODER_RESET_FUNC(encoding),                       \
        encoding##_decode,                                  \
        DECODER_INIT_FUNC(encoding),                        \
        DECODER_RESET_FUNC(encoding),                       \
    };                                                      \
                                                            \
    static struct PyMethodDef __methods[] = {               \
        {NULL, NULL},                                       \
    };                                                      \
                                                            \
    void                                                    \
    init_##encoding(void)                                   \
    {                                                       \
        PyObject    *codec;                                 \
        PyObject    *m = NULL, *mod = NULL, *o = NULL;      \
                                                            \
        m = Py_InitModule("_" #encoding, __methods);

#define MAPOPEN(locale)                                     \
    mod = PyImport_ImportModule("mapdata_" #locale);        \
    if (mod == NULL) goto errorexit;                        \
    if (
#define IMPORTMAP_ENCDEC(charset)                           \
    importmap(mod, "__map_" #charset, &charset##encmap,     \
        &charset##decmap) ||
#define IMPORTMAP_ENC(charset)                              \
    importmap(mod, "__map_" #charset, &charset##encmap,     \
        NULL) ||
#define IMPORTMAP_DEC(charset)                              \
    importmap(mod, "__map_" #charset, NULL,                 \
        &charset##decmap) ||
#define MAPCLOSE()                                          \
    0) goto errorexit;                                      \
    Py_DECREF(mod);

#define END_CODEC_REGISTRY(encoding)                        \
    mod = PyImport_ImportModule("multibytecodec");          \
    if (mod == NULL) goto errorexit;                        \
        o = PyObject_GetAttrString(mod, "__create_codec");  \
    if (o == NULL || !PyCallable_Check(o))                  \
        goto errorexit;                                     \
                                                            \
    codec = createcodec(o, &__codec);                       \
    if (codec == NULL)                                      \
        goto errorexit;                                     \
    PyModule_AddObject(m, "codec", codec);                  \
    Py_DECREF(o); Py_DECREF(mod);                           \
                                                            \
    if (PyErr_Occurred())                                   \
        Py_FatalError("can't initialize the _" #encoding    \
            " module");                                     \
                                                            \
    return;                                                 \
                                                            \
errorexit:                                                  \
    Py_XDECREF(m);                                          \
    Py_XDECREF(mod);                                        \
    Py_XDECREF(o);                                          \
}

#define CODEC_REGISTRY(encoding)                            \
    BEGIN_CODEC_REGISTRY(encoding)                          \
    END_CODEC_REGISTRY(encoding)

#ifdef USING_BINARY_PAIR_SEARCH
static DBCHAR
find_pairencmap(ucs2_t body, ucs2_t modifier,
                struct pair_encodemap *haystack, int haystacksize)
{
    int      pos, min, max;
    ucs4_t   value = body << 16 | modifier;

    min = 0;
    max = haystacksize;

    for (pos = haystacksize >> 1; min != max; pos = (min + max) >> 1)
        if (value < haystack[pos].uniseq) {
            if (max == pos) break;
            else max = pos;
        } else if (value > haystack[pos].uniseq) {
            if (min == pos) break;
            else min = pos;
        } else  
            break;  

    if (value == haystack[pos].uniseq)
        return haystack[pos].code;
    else    
        return DBCINV; 
}
#endif

#ifndef CODEC_WITHOUT_MAPS
static int
importmap(PyObject *mod, const char *symbol,
          const struct unim_index **encmap, const struct dbcs_index **decmap)
{
    PyObject    *o;

    o = PyObject_GetAttrString(mod, (char*)symbol);
    if (o == NULL)
        return -1;
    else if (!PyCObject_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "map data must be a CObject.");
        return -1;
    } else {
        struct dbcs_map *map;
        map = PyCObject_AsVoidPtr(o);
        if (encmap != NULL)
            *encmap = map->encmap;
        if (decmap != NULL)
            *decmap = map->decmap;
        Py_DECREF(o);
    }

    return 0;
}
#endif

static PyObject *
createcodec(PyObject *cofunc, MultibyteCodec *codec)
{
    PyObject    *args, *r;

    args = PyTuple_New(1);
    if (args == NULL) return NULL;
    PyTuple_SET_ITEM(args, 0, PyCObject_FromVoidPtr(codec, NULL));

    r = PyObject_CallObject(cofunc, args);
    Py_DECREF(args);

    return r;
}

/*
 * ex: ts=8 sts=4 et
 */
