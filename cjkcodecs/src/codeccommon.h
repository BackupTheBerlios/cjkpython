/*
 * codeccommon.h: Common Codec Routines
 *
 * Copyright (C) 2003-2004 Hye-Shik Chang <perky@FreeBSD.org>.
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
 * $Id: codeccommon.h,v 1.6 2004/06/16 06:04:18 perky Exp $
 */

#include "Python.h"
#include "multibytecodec.h"
#include "multibytecodec_compat.h"

#ifdef STRICT_BUILD
#define STRICT_SUFX     "_strict"
#else
#define STRICT_SUFX
#endif

#define ENCMAP(encoding)                                    \
    static const encode_map *encoding##_encmap;
#define DECMAP(encoding)                                    \
    static const decode_map *encoding##_decmap;

#define ENCODER_INIT(encoding)                              \
    static int encoding##_encode_init(                      \
        MultibyteCodec_State *state)
#define ENCODER(encoding)                                   \
    static int encoding##_encode(                           \
        MultibyteCodec_State *state,                        \
        const Py_UNICODE **inbuf, size_t inleft,            \
        unsigned char **outbuf, size_t outleft, int flags)
#define ENCODER_RESET(encoding)                             \
    static int encoding##_encode_reset(                     \
        MultibyteCodec_State *state,                        \
        unsigned char **outbuf, size_t outleft)

#define DECODER_INIT(encoding)                              \
    static int encoding##_decode_init(                      \
        MultibyteCodec_State *state)
#define DECODER(encoding)                                   \
    static int encoding##_decode(                           \
        MultibyteCodec_State *state,                        \
        const unsigned char **inbuf, size_t inleft,         \
        Py_UNICODE **outbuf, size_t outleft)
#define DECODER_RESET(encoding)                             \
    static int encoding##_decode_reset(                     \
        MultibyteCodec_State *state)

#if Py_UNICODE_SIZE == 4
#define UCS4INVALID(code)                                   \
    if ((code) > 0xFFFF)                                    \
        return 1;
#else
#define UCS4INVALID(code)                                   \
    if (0) ;
#endif

#define NEXT_IN(i)                                          \
    (*inbuf) += (i);                                        \
    (inleft) -= (i);
#define NEXT_OUT(o)                                         \
    (*outbuf) += (o);                                       \
    (outleft) -= (o);
#define NEXT(i, o)  NEXT_IN(i) NEXT_OUT(o)

#define RESERVE_INBUF(n)                                    \
    if (inleft < (n))                                       \
        return MBERR_TOOFEW;
#define RESERVE_OUTBUF(n)                                   \
    if (outleft < (n))                                      \
        return MBERR_TOOSMALL;

#define IN1 ((*inbuf)[0])
#define IN2 ((*inbuf)[1])
#define IN3 ((*inbuf)[2])
#define IN4 ((*inbuf)[3])

#define OUT1(c) ((*outbuf)[0]) = (c);
#define OUT2(c) ((*outbuf)[1]) = (c);
#define OUT3(c) ((*outbuf)[2]) = (c);
#define OUT4(c) ((*outbuf)[3]) = (c);

#define WRITE1(c1)                                          \
    RESERVE_OUTBUF(1)                                       \
    (*outbuf)[0] = (c1);
#define WRITE2(c1, c2)                                      \
    RESERVE_OUTBUF(2)                                       \
    (*outbuf)[0] = (c1);                                    \
    (*outbuf)[1] = (c2);
#define WRITE3(c1, c2, c3)                                  \
    RESERVE_OUTBUF(3)                                       \
    (*outbuf)[0] = (c1);                                    \
    (*outbuf)[1] = (c2);                                    \
    (*outbuf)[2] = (c3);
#define WRITE4(c1, c2, c3, c4)                              \
    RESERVE_OUTBUF(4)                                       \
    (*outbuf)[0] = (c1);                                    \
    (*outbuf)[1] = (c2);                                    \
    (*outbuf)[2] = (c3);                                    \
    (*outbuf)[3] = (c4);

#if Py_UNICODE_SIZE == 2
# define PUTUCS4(c)                                         \
    RESERVE_OUTBUF(2)                                       \
    (*outbuf)[0] = 0xd800 + (((c) - 0x10000) >> 10);        \
    (*outbuf)[1] = 0xdc00 + (((c) - 0x10000) & 0x3ff);      \
    NEXT_OUT(2)
#else
# define PUTUCS4(c)                                         \
    RESERVE_OUTBUF(1)                                       \
    **outbuf = (Py_UNICODE)(c);                             \
    NEXT_OUT(1)
#endif

#define _TRYMAP_ENC(m, assi, val)                           \
    if ((m)->map != NULL && (val) >= (m)->bottom &&         \
        (val)<= (m)->top && ((assi) = (m)->map[(val) -      \
            (m)->bottom]) != NOCHAR)
#define TRYMAP_ENC(charset, assi, uni)                      \
    _TRYMAP_ENC(&charset##_encmap[(uni) >> 8], assi, (uni) & 0xff)
#define _TRYMAP_DEC(m, assi, val)                           \
    if ((m)->map != NULL && (val) >= (m)->bottom &&         \
        (val)<= (m)->top && ((assi) = (m)->map[(val) -      \
            (m)->bottom]) != UNIINV)
#define TRYMAP_DEC(charset, assi, c1, c2)                   \
    _TRYMAP_DEC(&charset##_decmap[c1], assi, c2)

#if Py_UNICODE_SIZE == 2
#define DECODE_SURROGATE(c)                                 \
    if (c >> 10 == 0xd800 >> 10) { /* high surrogate */     \
        RESERVE_INBUF(2)                                    \
        if (IN2 >> 10 == 0xdc00 >> 10) { /* low surrogate */ \
            c = 0x10000 + ((ucs4_t)(c - 0xd800) << 10) +    \
                          ((ucs4_t)(IN2) - 0xdc00);         \
        }                                                   \
    }
#define GET_INSIZE(c)   ((c) > 0xffff ? 2 : 1)
#else
#define DECODE_SURROGATE(c) {;}
#define GET_INSIZE(c)   1
#endif

#ifdef USING_BINARY_PAIR_SEARCH
static DBCHAR find_pairencmap(ucs2_t , ucs2_t, struct pair_encodemap *, int);
#endif

/*
 * ex: ts=8 sts=4 et
 */
