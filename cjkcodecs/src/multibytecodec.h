/*
 * multibytecodec.h: Common Multibyte Codec Implementation
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
 * $Id: multibytecodec.h,v 1.1 2003/09/24 17:44:54 perky Exp $
 */

#ifndef _PYTHON_MULTIBYTECODEC_H_
#define _PYTHON_MULTIBYTECODEC_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "cjkcommon.h"

typedef union {
    void            *p;
    int              i;
    unsigned char    c[8];
    ucs2_t           u2[4];
    ucs4_t           u4[2];
} MultibyteCodec_State;

typedef int (*mbencode_func)(MultibyteCodec_State *state,
                             const Py_UNICODE **inbuf, size_t inleft,
                             unsigned char **outbuf, size_t outleft,
                             int flags);
typedef int (*mbencodeinit_func)(MultibyteCodec_State *state);
typedef int (*mbencodereset_func)(MultibyteCodec_State *state,
                             unsigned char **outbuf, size_t outleft);
typedef int (*mbdecode_func)(MultibyteCodec_State *state,
                             const unsigned char **inbuf, size_t inleft,
                             Py_UNICODE **outbuf, size_t outleft);
typedef int (*mbdecodeinit_func)(MultibyteCodec_State *state);
typedef int (*mbdecodereset_func)(MultibyteCodec_State *state);

typedef struct {
    const char          *encoding;
    mbencode_func        encode;
    mbencodeinit_func    encinit;
    mbencodereset_func   encreset;
    mbdecode_func        decode;
    mbdecodeinit_func    decinit;
    mbdecodereset_func   decreset;
} MultibyteCodec;

typedef struct {
    PyObject_HEAD
    MultibyteCodec    *codec;
} MultibyteCodecObject;

#define MAXDECPENDING   8
typedef struct {
    PyObject_HEAD
    MultibyteCodec          *codec;
    MultibyteCodec_State     state;
    unsigned char    pending[MAXDECPENDING];
    int              pendingsize;
    PyObject        *stream, *errors;
} MultibyteStreamReaderObject;

#define MAXENCPENDING   2
typedef struct {
    PyObject_HEAD
    MultibyteCodec          *codec;
    MultibyteCodec_State     state;
    Py_UNICODE       pending[MAXENCPENDING];
    int              pendingsize;
    PyObject        *stream, *errors;
} MultibyteStreamWriterObject;

/* positive values for illegal sequences */
#define MBERR_TOOSMALL      (-1) /* insufficient output buffer space */
#define MBERR_TOOFEW        (-2) /* incomplete input buffer */
#define MBERR_INTERNAL      (-3) /* internal runtime error */

#define ERROR_STRICT        (PyObject *)(1)
#define ERROR_IGNORE        (PyObject *)(2)
#define ERROR_REPLACE       (PyObject *)(3)
#define ERROR_MAX           ERROR_REPLACE

#define MBENC_FLUSH         0x0001 /* encode all characters encodable */
#define MBENC_MAX           MBENC_FLUSH

#ifdef __cplusplus
}
#endif
#endif

/*
 * ex: ts=8 sts=4 et
 */
