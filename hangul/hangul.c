/*
 * Copyright (C) 2002-2003 Hye-Shik Chang <perky@FreeBSD.org>.
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
 * $Id: hangul.c,v 1.1 2003/09/21 13:23:43 perky Exp $
 */

static char *version = "$Revision: 1.1 $";

#include "Python.h"

#if defined(__GNU_C__) || defined(__MSC__)
# define inline __inline
#else
# define inline
#endif

enum { /* Jaeum Codes on U+3100 */
    G = 0x3131, GG, GS, N, NJ, NH, D, DD, L, LG, LM, LB,
    LS, LT, LP, LH, M, B, BB, BS, S, SS, NG, J, JJ, C, K, T, P, H
};

enum { /* Moeum Codes on U+3100 */
    A = 0x314f, AE, YA, YAE, EO, E, YEO, YE, O, WA, WAE, OE, YO,
    U, WEO, WE, WI, YU, EU, YI, I
};

#define NCHOSEONG       19
#define NJUNGSEONG      21
#define NJONGSEONG      28

#define NJAEUM          30
#define NMOEUM          21

#define JAEUM_BOTTOM    G
#define JAEUM_TOP       H
#define MOEUM_BOTTOM    A
#define MOEUM_TOP       I

#define HANGUL_BOTTOM   0xac00
#define HANGUL_TOP      0xd7a3

#define JBASE_CHOSEONG  0x1100
#define JBASE_JUNGSEONG 0x1161
#define JBASE_JONGSEONG 0x11A8
#define CHOSEONG_FILLER 0x115f
#define JUNGSEONG_FILLER 0x1160

static PyObject *UniNull, *UniSpace;
static PyObject *ErrorObject;

#define MAX_MULTIJAMO   2
typedef struct _JAMOTYPE {
    char        *name;
    Py_UNICODE   code;
    Py_UNICODE   multi[MAX_MULTIJAMO];
    signed char  orders[3]; /* cho, jung, jong */
} JAMOTYPE;

#define CODE(c)         #c,c
#define NOMULTI         { 0, 0}
#define J_C             { 0,-1,-1}
#define J_J             {-1,-1, 0}
#define J_CJ            { 0,-1, 0}
#define M_J             {-1, 0,-1}
static JAMOTYPE jamos[] = {
/* JAEUM */
    { CODE(G),  NOMULTI, J_CJ }, { CODE(GG), {G, G,}, J_CJ },
    { CODE(GS), {G, S,}, J_J  }, { CODE(N),  NOMULTI, J_CJ },
    { CODE(NJ), {N, J,}, J_J  }, { CODE(NH), {N, H,}, J_J  },
    { CODE(D),  NOMULTI, J_CJ }, { CODE(DD), {D, D,}, J_C  },
    { CODE(L),  NOMULTI, J_CJ }, { CODE(LG), {L, G,}, J_J  },
    { CODE(LM), {L, M,}, J_J  }, { CODE(LB), {L, B,}, J_J  },
    { CODE(LS), {L, S,}, J_J  }, { CODE(LT), {L, T,}, J_J  },
    { CODE(LP), {L, P,}, J_J  }, { CODE(LH), {L, H,}, J_J  },
    { CODE(M),  NOMULTI, J_CJ }, { CODE(B),  NOMULTI, J_CJ },
    { CODE(BB), {B, B,}, J_C  }, { CODE(BS), {B, S,}, J_J  },
    { CODE(S),  NOMULTI, J_CJ }, { CODE(SS), {S, S,}, J_CJ },
    { CODE(NG), NOMULTI, J_CJ }, { CODE(J),  NOMULTI, J_CJ },
    { CODE(JJ), {J, J,}, J_C  }, { CODE(C),  NOMULTI, J_CJ },
    { CODE(K),  NOMULTI, J_CJ }, { CODE(T),  NOMULTI, J_CJ },
    { CODE(P),  NOMULTI, J_CJ }, { CODE(H),  NOMULTI, J_CJ },
/* MOEUM */
    { CODE(A),  NOMULTI, M_J  }, { CODE(AE), NOMULTI, M_J  },
    { CODE(YA), NOMULTI, M_J  }, { CODE(YAE),NOMULTI, M_J  },
    { CODE(EO), NOMULTI, M_J  }, { CODE(E),  NOMULTI, M_J  },
    { CODE(YEO),NOMULTI, M_J  }, { CODE(YE), NOMULTI, M_J  },
    { CODE(O),  NOMULTI, M_J  }, { CODE(WA), {O,  A}, M_J  },
    { CODE(WAE),{O, AE}, M_J  }, { CODE(OE), {O,  I}, M_J  },
    { CODE(YO), NOMULTI, M_J  }, { CODE(U),  NOMULTI, M_J  },
    { CODE(WEO),{U, EO}, M_J  }, { CODE(WE), {U,  E}, M_J  },
    { CODE(WI), {U,  I}, M_J  }, { CODE(YU), NOMULTI, M_J  },
    { CODE(EU), NOMULTI, M_J  }, { CODE(YI), {EU, I}, M_J  },
    { CODE(I),  NOMULTI, M_J  },
/* END MARKER */
    { 0, 0, NOMULTI, {0,} },
};
#undef NOMULTI
#undef CODE

static JAMOTYPE     *jamo_choseong[NCHOSEONG],
                    *jamo_jungseong[NJUNGSEONG],
                    *jamo_jongseong[NJONGSEONG];

#define getJamotype(c)          jamos[(c)-JAEUM_BOTTOM]
#define isJaeum(c)              (JAEUM_BOTTOM <= (c) && (c) <= JAEUM_TOP)
#define isMoeum(c)              (MOEUM_BOTTOM <= (c) && (c) <= MOEUM_TOP)
#define isJamo(c)               (isJaeum(c) || isMoeum(c))
#define isHangulSyllable(c)     (HANGUL_BOTTOM <= (c) && (c) <= HANGUL_TOP)
#define isChoseong(c)           (getJamotype(c).orders[0] >= 0)
#define isJungseong(c)          (getJamotype(c).orders[1] >= 0)
#define isJongseong(c)          (getJamotype(c).orders[2] >= 0)
#define getChoseongOrder(c)     (getJamotype(c).orders[0])
#define getJungseongOrder(c)    (getJamotype(c).orders[1])
#define getJongseongOrder(c)    (getJamotype(c).orders[2])

static inline JAMOTYPE *
TryMultiJamo(Py_UNICODE left, Py_UNICODE right, int pos)
{
    int i;

    for (i=0; jamos[i].code != 0; i++)
        if (jamos[i].orders[pos] >= 0 &&
                jamos[i].multi[0] == left &&
                jamos[i].multi[1] == right)
            return &jamos[i];

    return NULL;
}

static inline int
GetUnicodeDataFromObject(PyObject **obj, Py_UNICODE **data, int *datalen)
{
    PyObject *tobj;

    tobj = PyUnicode_FromObject(*obj); /* *obj gets new reference */
    Py_DECREF(*obj);
    *obj = tobj;
    if (tobj == NULL)
        return -1;

    *data = PyUnicode_AS_UNICODE(*obj);
    *datalen = PyUnicode_GET_SIZE(*obj);
    return 0;
}

static char hangul_isJaeum__doc__[] =
"isJaeum(code): Verify whether the code is Jaeum.";

static PyObject *
hangul_isJaeum(PyObject *self, PyObject *args)
{
    PyObject    *obj;
    Py_UNICODE  *code;
    int          codelen, istrue = 0;

    if (!PyArg_ParseTuple(args, "O:isJaeum", &obj))
        return NULL;

    Py_INCREF(obj);
    if (GetUnicodeDataFromObject(&obj, &code, &codelen) != 0)
        return NULL;

    if (codelen)
        for (istrue = 1; codelen--; code++)
            if (!isJaeum(*code)) {
                istrue = 0;
                break;
            }

    Py_DECREF(obj);
    if (istrue) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static char hangul_isMoeum__doc__[] =
"isMoeum(code): Verify whether the code is Moeum.";

static PyObject *
hangul_isMoeum(PyObject *self, PyObject *args)
{
    PyObject    *obj;
    Py_UNICODE  *code;
    int          codelen, istrue = 0;

    if (!PyArg_ParseTuple(args, "O:isMoeum", &obj))
        return NULL;

    Py_INCREF(obj);
    if (GetUnicodeDataFromObject(&obj, &code, &codelen) != 0)
        return NULL;

    if (codelen)
        for (istrue = 1; codelen--; code++)
            if (!isMoeum(*code)) {
                istrue = 0;
                break;
            }

    Py_DECREF(obj);
    if (istrue) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static char hangul_ishangul__doc__[] =
"ishangul(code): Verify whether the code is hangul.";

static PyObject *
hangul_ishangul(PyObject *self, PyObject *args)
{
    PyObject    *obj;
    Py_UNICODE  *code;
    int          codelen, istrue = 0;

    if (!PyArg_ParseTuple(args, "O:ishangul", &obj))
        return NULL;

    Py_INCREF(obj);
    if (GetUnicodeDataFromObject(&obj, &code, &codelen) != 0)
        return NULL;

    if (codelen)
        for (istrue = 1; codelen--; code++)
            if (!(isHangulSyllable(*code) || isJaeum(*code)
                  || isMoeum(*code))) {
                istrue = 0;
                break;
            }

    Py_DECREF(obj);
    if (istrue) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static char hangul_join__doc__[] =
"join([jamoelement, ...]): Assemble hangul syllable from jamos.";

#define MAXJOINELEMENTS     6
static PyObject *
hangul_join(PyObject *self, PyObject *args)
{
    PyObject        *argseq;
    Py_UNICODE       elems[MAXJOINELEMENTS], cho, jung, jong;
    int              i, len, pos;

    if (!PyArg_ParseTuple(args, "O:join", &argseq))
        return NULL;

    if (PyString_Check(argseq)) {
        argseq = PyUnicode_FromObject(argseq);
        if (argseq == NULL)
            return NULL;
    } else {
        Py_INCREF(argseq); /* to give equivalent refcount with above */
    }

    if (PySequence_Check(argseq)) {
        len = PySequence_Length(argseq);
        if (len >= MAXJOINELEMENTS) {
            PyErr_Format(PyExc_ValueError, "input sequence is too big");
            goto errorexit;
        }
        for (i=0, pos=0; i < len; i++) {
            PyObject    *tv;
            Py_UNICODE  *code;
            int          codelen;

            tv = PySequence_GetItem(argseq, i);
            if (tv == NULL)
                goto errorexit;
            else if (GetUnicodeDataFromObject(&tv, &code, &codelen) != 0)
                goto errorexit;
            else if (codelen > 1) {
                PyErr_Format(PyExc_ValueError,
                    "each element must be a character");
                Py_DECREF(tv);
                goto errorexit;
            }
            else if (codelen == 1) {
                if (!isJamo(*code)) {
                    PyErr_Format(PyExc_ValueError,
                            "u'\\u%04x' is not a hangul jamo", *code);
                    Py_DECREF(tv);
                    goto errorexit;
                }
                elems[pos++] = *code;
            }
            Py_DECREF(tv);
        }
        len = pos;
    }
    else {
        PyErr_Format(PyExc_ValueError,
            "need sequence object with 3 unicode character elements");
        goto errorexit;
    }

    cho = jung = jong = 0;
    for (i=0; i < len; i++) {
        JAMOTYPE *jamo;

        if (isJaeum(elems[i])) {
            if (jung != 0) {
                if (jong != 0) {
                    jamo = TryMultiJamo(jong, elems[i], 2);
                    if (jamo == NULL)
                        goto invalidseq;
                    jong = jamo->code;
                }
                else {
                    if (!isJongseong(elems[i]))
                        goto invalidseq;
                    jong = elems[i];
                }
            }
            else if (cho != 0) {
                jamo = TryMultiJamo(cho, elems[i], 0);
                if (jamo == NULL)
                    goto invalidseq;
                cho = jamo->code;
            }
            else {
                if (!isChoseong(elems[i]))
                    goto invalidseq;
                cho = elems[i];
            }
        } else { /* Moeum */
            if (jong != 0)
                goto invalidseq;
            else if (jung != 0) {
                jamo = TryMultiJamo(jung, elems[i], 1);
                if (jamo == NULL)
                    goto invalidseq;
                jung = jamo->code;
            }
            else /* allow single moeum */
                jung = elems[i];
        }
    }

    if (jong != 0 && (jung == 0 || cho == 0))
        goto invalidseq;
    else if (jong == 0 && cho == 0 && jung != 0) /* single moeum */
        return PyUnicode_FromUnicode(&jung, 1);
    else if (cho != 0 && jung == 0) /* single jaeum (non-jongseong) */
        return PyUnicode_FromUnicode(&cho, 1);
    else if (cho != 0 && jung != 0) {
        Py_UNICODE  c;

        c = getChoseongOrder(cho) * NJUNGSEONG;
        c += getJungseongOrder(jung);
        c *= NJONGSEONG;
        if (jong != 0)
            c += getJongseongOrder(jong);
        c += HANGUL_BOTTOM;
        return PyUnicode_FromUnicode(&c, 1);
    } else {
        Py_INCREF(UniSpace);
        return UniSpace;
    }

invalidseq:
    PyErr_Format(ErrorObject, "invalid hangul jamo sequence");
errorexit:
    Py_DECREF(argseq);
    return NULL;
}

static char hangul_split__doc__[] =
"split(code): Disassemble hangul syllable into jamos.";

static PyObject *
hangul_split(PyObject *self, PyObject *args)
{
    Py_UNICODE      *code;
    PyObject        *r, *obj;
    int              codelen;

    if (!PyArg_ParseTuple(args, "O:split", &obj))
        return NULL;

    Py_INCREF(obj);
    if (GetUnicodeDataFromObject(&obj, &code, &codelen) != 0)
        return NULL;

    if (codelen < 1) {
        PyErr_Format(PyExc_ValueError, "need not null unicode string");
        r = NULL;
    }
    else if (isHangulSyllable(*code)) {
        Py_UNICODE       cho, jung, jong;
        Py_UNICODE       hseq, t;
        PyObject        *jongobj;
        
        hseq = *code - HANGUL_BOTTOM;

        cho  = jamo_choseong[hseq / (NJUNGSEONG * NJONGSEONG)]->code;
        jung = jamo_jungseong[(hseq / NJONGSEONG) % NJUNGSEONG]->code;

        if ((t = hseq % NJONGSEONG)) {
            jong = jamo_jongseong[t]->code;
            jongobj = PyUnicode_FromUnicode(&jong, 1);
        } else {
            jongobj = UniNull;
            Py_INCREF(UniNull);
        }

        r = PyTuple_New(3);
        PyTuple_SET_ITEM(r, 0, PyUnicode_FromUnicode(&cho, 1));
        PyTuple_SET_ITEM(r, 1, PyUnicode_FromUnicode(&jung, 1));
        PyTuple_SET_ITEM(r, 2, jongobj);
    }
    else if (isJaeum(*code)) {
        r = PyTuple_New(3);
        PyTuple_SET_ITEM(r, 0, PyUnicode_FromUnicode(code, 1));
        PyTuple_SET_ITEM(r, 1, UniNull); Py_INCREF(UniNull);
        PyTuple_SET_ITEM(r, 2, UniNull); Py_INCREF(UniNull);
    }
    else if (isMoeum(*code)) {
        r = PyTuple_New(3);
        PyTuple_SET_ITEM(r, 0, UniNull); Py_INCREF(UniNull);
        PyTuple_SET_ITEM(r, 1, PyUnicode_FromUnicode(code, 1));
        PyTuple_SET_ITEM(r, 2, UniNull); Py_INCREF(UniNull);
    }
    else {
        PyErr_Format(ErrorObject, "not a hangul code");
        r = NULL;
    }

    Py_DECREF(obj);
    return r;
}

static char hangul_conjoin__doc__[] =
"conjoin(unicodestring): conjoin unicode johab string into unicode "
"syllable string";

static PyObject *
hangul_conjoin(PyObject *self, PyObject *args)
{
    PyObject        *r, *obj;
    Py_UNICODE      *code, *dst, *dstorg, c;
    int              cho, jung, jong;
    int              codelen, i;

    if (!PyArg_ParseTuple(args, "O:conjoin", &obj))
        return NULL;

    Py_INCREF(obj);
    if (GetUnicodeDataFromObject(&obj, &code, &codelen) != 0)
        return NULL;

    dstorg = dst = PyMem_New(Py_UNICODE, codelen);

    for (i = 0; i < codelen; i++) {
        c = code[i];
        if ((JBASE_CHOSEONG <= c && c <= 0x1112) || c == CHOSEONG_FILLER) {
            if (codelen > i+1 && JUNGSEONG_FILLER <= code[i+1]
                && code[i+1] <= 0x1175) {

                cho     = (c == CHOSEONG_FILLER) ? -1 : c - JBASE_CHOSEONG;
                jung    = (code[i+1] == JUNGSEONG_FILLER) ? -1 :
                           code[i+1] - JBASE_JUNGSEONG;

                if (codelen > i+2 && JBASE_JONGSEONG <= code[i+2]
                    && code[i+2] <= 0x11c2) {
                    jong = code[i+2] - JBASE_JONGSEONG + 1;
                    i += 2;
                } else {
                    jong = 0; i++;
                }

                if (jong && (cho == -1 || jung == -1)) {
                                    /* can't trans to syllable */
                    if (cho >= 0)
                        *(dst++) = jamo_choseong[cho]->code;
                    if (jung >= 0)
                        *(dst++) = jamo_jungseong[jung]->code;
                    *(dst++) = jamo_jongseong[jong]->code;
                }
                else if (cho == -1) /* jungseong only */
                    *(dst++) = jamo_jungseong[jung]->code;
                else if (jung == -1) /* choseong only */
                    *(dst++) = jamo_choseong[cho]->code;
                else /* full set */
                    *(dst++) = HANGUL_BOTTOM +
                               (cho * NJUNGSEONG + jung) * NJONGSEONG + jong;
            }
            else if (c != CHOSEONG_FILLER) /* choseong only */
                *(dst++) = jamo_choseong[c-JBASE_CHOSEONG]->code;
        }
        else if (JBASE_JUNGSEONG <= c && c <= 0x1175) /* jungseong only */
            *(dst++) = jamo_jungseong[c-JBASE_JUNGSEONG]->code;
        else
            *(dst++) = c;
    }

    r = PyUnicode_FromUnicode(dstorg, dst-dstorg);
    PyMem_Del(dstorg);
    Py_DECREF(obj);

    return r;
}


static char hangul_disjoint__doc__[] =
"disjoint(unicodestring): disjoint unicode syllable string "
"into unicode johab string";

static PyObject *
hangul_disjoint(PyObject *self, PyObject *args)
{
    Py_UNICODE      *code, *dst, *dstorg, c;
    PyObject        *r, *obj;
    int              codelen, i;

    if (!PyArg_ParseTuple(args, "O:disjoint", &obj))
        return NULL;

    Py_INCREF(obj);
    if (GetUnicodeDataFromObject(&obj, &code, &codelen) != 0)
        return NULL;

    dstorg = dst = PyMem_New(Py_UNICODE, codelen*3);

    for (i = 0; i < codelen; i++) {
        c = code[i];
        if (isHangulSyllable(c)) {
            int hseq;
            Py_UNICODE jong;

            hseq = c - HANGUL_BOTTOM;
            jong = hseq % NJONGSEONG;

            *(dst++) = hseq / (NJUNGSEONG * NJONGSEONG) + JBASE_CHOSEONG;
            *(dst++) = (hseq / NJONGSEONG) % NJUNGSEONG + JBASE_JUNGSEONG;
            if (jong)
                *(dst++) = jong + JBASE_JONGSEONG - 1;
        }
        else if (isJaeum(c) && isChoseong(c)) {
            *(dst++) = getChoseongOrder(c) + JBASE_CHOSEONG;
            *(dst++) = JUNGSEONG_FILLER;
        }
        else if (isMoeum(c)) {
            *(dst++) = CHOSEONG_FILLER;
            *(dst++) = getJungseongOrder(c) + JBASE_JUNGSEONG;
        } else
            *(dst++) = c;
    }

    r = PyUnicode_FromUnicode(dstorg, dst-dstorg);
    PyMem_Del(dstorg);

    return r;
}


static char pseudofinal[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 1 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  /* 2 */
    1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,  /* 3 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,  /* 4 */
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,  /* 5 */
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,  /* 6 */
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 7 */
};

static char hangul_format__doc__[] =
"format(fmt, arg1, arg2, ...) or format(fmt, kw1=arg1, kw2=arg2"
", ...):\nformat unicode string and fix korean suffixes after arguments";

static PyObject *
hangul_format(PyObject *self, PyObject *args, PyObject *kwargs)
{
/*--- Poor Structure of this function ;)
  hangul_format(fmt, *args, **kwargs)
    -> insert end fmtmarkers(U+115E which is not used by Unicode)
       after every format position
      -> PyUnicode_Format
        -> Fix and update hangul suffixes in place of fmtmarkers
          -> make PyObject and return.
 */
#define FMTMARKER 0x115E
    Py_UNICODE      *fmt, *fmtout, *fcur;
    PyObject        *r, *fmtobj;
    int              fmtsize;
    int              inpth, infmt, escape;

    {
        int     argsize;

        argsize = PyTuple_GET_SIZE(args);
        if (!argsize) {
            PyErr_Format(PyExc_TypeError, "needs unicode format string.");
            return NULL;
        }
        fmtobj = PyTuple_GET_ITEM(args, 0);
        Py_INCREF(fmtobj);
        if (GetUnicodeDataFromObject(&fmtobj, &fmt, &fmtsize) != 0)
            return NULL;

        if (!kwargs)
            args = PyTuple_GetSlice(args, 1, argsize);
    }

    fmtout = PyMem_New(Py_UNICODE, fmtsize + fmtsize/2);
    inpth = infmt = escape = 0;

    for (fcur = fmtout; fmtsize--; fmt++) {
        if (*fmt != FMTMARKER) /* skip bogus markers */
            *(fcur++) = *fmt;

        if (escape)
            escape = 0;
        else if (*fmt == '\\')
            escape = 1;
        else if (infmt) {
            if (!inpth && (('A' <= *fmt && *fmt <= 'Z')
                || ('a' <= *fmt && *fmt <= 'z'))) {
                *(fcur++) = FMTMARKER;
                infmt = 0;
            }
            else if (inpth && *fmt == ')')
                inpth = 0;
            else if (*fmt == '(')
                inpth = 1;
            else if (*fmt == '%')
                infmt = 0;
        }
        else if (*fmt == '%')
            infmt = 1;
    }

    r = PyUnicode_Format(
            PyUnicode_FromUnicode(fmtout, fcur-fmtout),
            kwargs ? kwargs : args
        );
    if (!kwargs) {
        Py_DECREF(args);
    }
    if (r == NULL)
        goto out;

    fmt       = PyUnicode_AS_UNICODE(r);
    fmtsize   = PyUnicode_GET_SIZE(r);

#define HAS_FINAL() (                                                   \
    (past = *(fmt-1)),                                                  \
    isHangulSyllable(past) ?                                            \
        ((past-HANGUL_BOTTOM) % NJONGSEONG > 0)                         \
        : (past < 0x80 ? pseudofinal[past] : 0)                         \
)

#define HAS_FINAL_OR_NOTSYL() (                                         \
    (past = *(fmt-1)),                                                  \
    isHangulSyllable(past) ?                                            \
        ((past-HANGUL_BOTTOM) % NJONGSEONG > 0)                         \
        : 1                                                             \
)

#define PROCESSSUFFIX(nofinal, existfinal)                              \
    if (next == nofinal || next == existfinal) {                        \
        *(fcur++) = HAS_FINAL() ? (existfinal) : (nofinal);             \
        fmtsize--; fmt++;                                               \
    }

#define PROCESSSUFFIX_IDA(jongseongadder, existfinal)                   \
    if (next == existfinal) {                                           \
        if (HAS_FINAL_OR_NOTSYL())                                      \
            *(fcur++) = existfinal;                                     \
        else                                                            \
            *(fcur-1) += jongseongadder;                                \
        fmtsize-=3; fmt+=3;                                             \
    }

    for (fcur = fmtout; fmtsize--; fmt++) {
        if (*fmt == FMTMARKER) {
            if (fcur > fmtout && fmtsize > 0) {
                Py_UNICODE past, next = *(fmt+1);

                if (next == '(' && fmtsize > 2 && *(fmt + 3) == ')') {
                                                    /* ida suffxes */
                    next = *(fmt + 2);
                    PROCESSSUFFIX_IDA(0, 0xc774) /* (I)DA */
                    else PROCESSSUFFIX_IDA(17, 0xc785) /* (IP)NIDA */
                    else PROCESSSUFFIX_IDA(4, 0xc778) /* (IN)- */
                }
                else if (0xac00 <= next && next <= 0xc774) {
                    PROCESSSUFFIX(0xb97c, 0xc744) /* REUL, EUL */
                    else PROCESSSUFFIX(0xb294, 0xc740) /* NEUN, EUN */
                    else PROCESSSUFFIX(0xac00, 0xc774) /* GA, I */
                    else PROCESSSUFFIX(0xc640, 0xacfc) /* WA, GWA */
                }
            }
        }
        else
            *(fcur++) = *fmt;
    }

#undef PROCESSSUFFIX
#undef PROCESSSUFFIX_IDA
#undef HAS_FINAL
#undef HAS_FINAL_OR_NOTSYL

    Py_DECREF(r);
    r = PyUnicode_FromUnicode(fmtout, fcur-fmtout);

out:
    PyMem_Free(fmtout);
    Py_DECREF(fmtobj);
    return r;
}

static struct PyMethodDef hangul_methods[] = {
    {"isJaeum",     (PyCFunction)hangul_isJaeum,
                    METH_VARARGS, hangul_isJaeum__doc__},
    {"isMoeum",     (PyCFunction)hangul_isMoeum,
                    METH_VARARGS, hangul_isMoeum__doc__},
    {"ishangul",    (PyCFunction)hangul_ishangul,
                    METH_VARARGS, hangul_ishangul__doc__},
    {"join",        (PyCFunction)hangul_join,
                    METH_VARARGS, hangul_join__doc__},
    {"split",       (PyCFunction)hangul_split,
                    METH_VARARGS, hangul_split__doc__},
    {"conjoin",     (PyCFunction)hangul_conjoin,
                    METH_VARARGS, hangul_conjoin__doc__},
    {"disjoint",    (PyCFunction)hangul_disjoint,
                    METH_VARARGS, hangul_disjoint__doc__},
    {"format",      (PyCFunction)hangul_format,
                    METH_VARARGS | METH_KEYWORDS, hangul_format__doc__},
    {NULL,          NULL},
};

#define SET_INTCONSTANT(dict, value) {          \
    PyObject *mp;                               \
    mp = PyInt_FromLong((long)value);           \
    PyDict_SetItemString(dict, #value, mp);     \
    Py_DECREF(mp);                              \
}
#define SET_STRCONSTANT(dict, value) {          \
    PyObject *mp;                               \
    mp = PyString_FromString(value);            \
    PyDict_SetItemString(dict, #value, mp);     \
    Py_DECREF(mp);                              \
}
#define SET_CHARCONSTANT(dict, value) {         \
    PyObject *mp;                               \
    mp = PyString_FromFormat("%c", value);      \
    PyDict_SetItemString(dict, #value, mp);     \
    Py_DECREF(mp);                              \
}
#define SET_UNICHARCONSTANT(dict, value) {      \
    PyObject *mp;                               \
    Py_UNICODE tmpuni = value;                  \
    mp = PyUnicode_FromUnicode(&tmpuni, 1);     \
    PyDict_SetItemString(dict, #value, mp);     \
    Py_DECREF(mp);                              \
}

void
inithangul(void)
{
    PyObject        *m, *d, *tmp;
    Py_UNICODE       tuni[2];
    int              i;

    m = Py_InitModule("hangul", hangul_methods);

    UniNull = PyUnicode_FromUnicode(NULL, 0);
    tuni[0] = 0x3000; /* Unicode Double-wide Space */
    UniSpace = PyUnicode_FromUnicode(tuni, 1);

    d = PyModule_GetDict(m);
    SET_INTCONSTANT(d, NCHOSEONG);
    SET_INTCONSTANT(d, NJUNGSEONG);
    SET_INTCONSTANT(d, NJONGSEONG);
    {
        PyObject    *Choseong, *Jungseong, *Jongseong;
        PyObject    *Jaeum, *Moeum;
        PyObject    *JaeumDict, *MoeumDict;
        PyObject    *JaeumCodes, *MoeumCodes;
        PyObject    *JaeumMulti, *MoeumMulti;
        int          cur_cho, cur_jung, cur_jong;
        int          cur_jaeum, cur_moeum;
        JAMOTYPE    *jamo;

        /* Bind Choseong, Jungseong, Jongseong lists */
        cur_cho = cur_jung = cur_jong = 0;
        Choseong  = PyList_New(NCHOSEONG);
        Jungseong = PyList_New(NJUNGSEONG);
        Jongseong = PyList_New(NJONGSEONG);
        PyDict_SetItemString(d, "Choseong", Choseong);
        PyDict_SetItemString(d, "Jungseong", Jungseong);
        PyDict_SetItemString(d, "Jongseong", Jongseong);
        jamo_jongseong[cur_jong] = NULL;
        Py_INCREF(UniNull);
        PyList_SET_ITEM(Jongseong, cur_jong++, UniNull);

        /* Create Jaeum and Moeum meta class */
        JaeumDict = PyDict_New();
        MoeumDict = PyDict_New();

        tmp = PyString_FromString("Jaeum");
        Jaeum = PyClass_New(NULL, JaeumDict, tmp);
        Py_DECREF(tmp);

        tmp = PyString_FromString("Moeum");
        Moeum = PyClass_New(NULL, MoeumDict, tmp);
        Py_DECREF(tmp);

        /* Bind meta class members */
        PyDict_SetItemString(d, "Jaeum", Jaeum);
        PyDict_SetItemString(d, "Moeum", Moeum);
        PyDict_SetItemString(JaeumDict, "Choseong", Choseong);
        PyDict_SetItemString(MoeumDict, "Jungseong", Jungseong);
        PyDict_SetItemString(JaeumDict, "Jongseong", Jongseong);

        /* Create Jaeum and Moeum Members */
        JaeumCodes = PyTuple_New(NJAEUM);
        MoeumCodes = PyTuple_New(NMOEUM);
        JaeumMulti = PyDict_New();
        MoeumMulti = PyDict_New();
        cur_jaeum = cur_moeum = 0;
        PyDict_SetItemString(JaeumDict, "Codes", JaeumCodes);
        PyDict_SetItemString(MoeumDict, "Codes", MoeumCodes);
        PyDict_SetItemString(JaeumDict, "Width", PyInt_FromLong(NJAEUM));
        PyDict_SetItemString(MoeumDict, "Width", PyInt_FromLong(NMOEUM));
        PyDict_SetItemString(JaeumDict, "MultiElement", JaeumMulti);
        PyDict_SetItemString(MoeumDict, "MultiElement", MoeumMulti);

        for (jamo = jamos; jamo->name; jamo++) {
            PyObject    *unijamo, *multicls;
            int          tuplen;

            tuni[0] = jamo->code;
            unijamo = PyUnicode_FromUnicode(tuni, 1);
            PyDict_SetItemString(d, jamo->name, unijamo);

            if (isJaeum(jamo->code)) {
                PyTuple_SET_ITEM(JaeumCodes, cur_jaeum++, unijamo);
                Py_INCREF(unijamo);
                if (isChoseong(jamo->code)) {
                    jamo->orders[0] = cur_cho;
                    jamo_choseong[cur_cho] = jamo;
                    PyList_SET_ITEM(Choseong,  cur_cho++, unijamo);
                    Py_INCREF(unijamo);
                    PyDict_SetItemString(JaeumDict, jamo->name, unijamo);
                }
                if (isJongseong(jamo->code)) {
                    jamo->orders[2] = cur_jong;
                    jamo_jongseong[cur_jong] = jamo;
                    PyList_SET_ITEM(Jongseong, cur_jong++, unijamo);
                    Py_INCREF(unijamo);
                    PyDict_SetItemString(JaeumDict, jamo->name, unijamo);
                }
                multicls = JaeumMulti;
            }
            else { /* Moeum */
                PyTuple_SET_ITEM(MoeumCodes, cur_moeum++, unijamo);
                Py_INCREF(unijamo);
                if (isJungseong(jamo->code)) {
                    jamo->orders[1] = cur_jung;
                    jamo_jungseong[cur_jung] = jamo;
                    PyList_SET_ITEM(Jungseong, cur_jung++, unijamo);
                    Py_INCREF(unijamo);
                    PyDict_SetItemString(MoeumDict, jamo->name, unijamo);
                }
                multicls = MoeumMulti;
            }
            if (jamo->multi[0]) {
                tuplen = jamo->multi[2] ? 3 : 2;
                tmp = PyTuple_New(tuplen);
                for (i = 0; i < tuplen; i++) {
                    tuni[0] = jamo->multi[i];
                    PyTuple_SET_ITEM(tmp, i, PyUnicode_FromUnicode(tuni, 1));
                }
                PyDict_SetItem(multicls, unijamo, tmp);
                Py_DECREF(tmp);
            }
            Py_DECREF(unijamo);
        }

        Py_DECREF(Choseong);     Py_DECREF(Jungseong);    Py_DECREF(Jongseong);
        Py_DECREF(JaeumDict);   Py_DECREF(MoeumDict);
        Py_DECREF(JaeumCodes);  Py_DECREF(MoeumCodes);
        Py_DECREF(JaeumMulti);  Py_DECREF(MoeumMulti);
    }

    tmp = PyTuple_New(2);
    tuni[0] = HANGUL_BOTTOM;
    PyTuple_SET_ITEM(tmp, 0, PyUnicode_FromUnicode(tuni, 1));
    tuni[0] = HANGUL_TOP;
    PyTuple_SET_ITEM(tmp, 1, PyUnicode_FromUnicode(tuni, 1));
    PyDict_SetItemString(d, "ZONE", tmp);
    Py_DECREF(tmp);

    SET_UNICHARCONSTANT(d, JBASE_CHOSEONG);    
    SET_UNICHARCONSTANT(d, JBASE_JUNGSEONG);    
    SET_UNICHARCONSTANT(d, JBASE_JONGSEONG);    
    SET_UNICHARCONSTANT(d, CHOSEONG_FILLER);    
    SET_UNICHARCONSTANT(d, JUNGSEONG_FILLER);    

    PyDict_SetItemString(d, "Null", UniNull);
    PyDict_SetItemString(d, "Space", UniSpace);

    tmp = PyString_FromString(version);
    PyDict_SetItemString(d, "__version__", tmp);
    Py_DECREF(tmp);

    ErrorObject = PyErr_NewException("hangul.UnicodeHangulError", NULL, NULL);
    PyDict_SetItemString(d, "UnicodeHangulError", ErrorObject);

    /* Check for errors */
    if (PyErr_Occurred())
        Py_FatalError("can't initialize the hangul module");
}

/*
 * $Id: hangul.c,v 1.1 2003/09/21 13:23:43 perky Exp $
 * ex: ts=8 sts=4 et
 */
