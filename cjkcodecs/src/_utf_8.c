/*
 * _utf_8.c: the UTF-8 codec (RFC2279)
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
 * $Id: _utf_8.c,v 1.2 2003/12/31 05:46:55 perky Exp $
 */

#include "codeccommon.h"

ENCODER(utf_8)
{
    while (inleft > 0) {
        ucs4_t      c = **inbuf;
        int         outsize, insize = 1;

        if (c < 0x80) outsize = 1;
        else if (c < 0x800) outsize = 2;
        else {
#if Py_UNICODE_SIZE == 2
            if (c >> 10 == 0xd800 >> 10) { /* high surrogate */
                if (inleft < 2) {
                    if (!(flags & MBENC_FLUSH))
                        return MBERR_TOOFEW;
                } else if ((*inbuf)[1] >> 10 == 0xdc00 >> 10) {
                    /* low surrogate */
                    c = 0x10000 + ((c - 0xd800) << 10) +
                                  ((ucs4_t)((*inbuf)[1]) - 0xdc00);
                    insize = 2;
                }
            }
#endif
            if (c < 0x10000) outsize = 3;
            else if (c < 0x200000) outsize = 4;
            else if (c < 0x4000000) outsize = 5;
            else outsize = 6;
        }

        RESERVE_OUTBUF(outsize)

        switch (outsize) {
        case 6:
            (*outbuf)[5] = 0x80 | (c & 0x3f);
            c = c >> 6;
            c |= 0x4000000;
            /* FALLTHROUGH */
        case 5:
            (*outbuf)[4] = 0x80 | (c & 0x3f);
            c = c >> 6;
            c |= 0x200000;
            /* FALLTHROUGH */
        case 4:
            (*outbuf)[3] = 0x80 | (c & 0x3f);
            c = c >> 6;
            c |= 0x10000;
            /* FALLTHROUGH */
        case 3:
            (*outbuf)[2] = 0x80 | (c & 0x3f);
            c = c >> 6;
            c |= 0x800;
            /* FALLTHROUGH */
        case 2:
            (*outbuf)[1] = 0x80 | (c & 0x3f);
            c = c >> 6;
            c |= 0xc0;
            /* FALLTHROUGH */
        case 1:
            (*outbuf)[0] = c;
        }

        NEXT(insize, outsize)
    }

    return 0;
}

DECODER(utf_8)
{
    while (inleft > 0) {
        unsigned char    c = **inbuf;

        RESERVE_OUTBUF(1)

        if (c < 0x80) {
            (*outbuf)[0] = (unsigned char)c;
            NEXT(1, 1)
        } else if (c < 0xc2) {
            return 1;
        } else if (c < 0xe0) {
            unsigned char    c2;

            RESERVE_INBUF(2)
            c2 = (*inbuf)[1];
            if (!((c2 ^ 0x80) < 0x40))
                return 2;
            **outbuf = ((Py_UNICODE)(c & 0x1f) << 6) | (Py_UNICODE)(c2 ^ 0x80);
            NEXT(2, 1)
        } else if (c < 0xf0) {
            unsigned char    c2, c3;

            RESERVE_INBUF(3)
            c2 = (*inbuf)[1]; c3 = (*inbuf)[2];
            if (!((c2 ^ 0x80) < 0x40 &&
                  (c3 ^ 0x80) < 0x40 && (c >= 0xe1 || c2 >= 0xa0)))
                return 3;
            **outbuf = ((Py_UNICODE)(c & 0x0f) << 12)
                           | ((Py_UNICODE)(c2 ^ 0x80) << 6)
                           | (Py_UNICODE)(c3 ^ 0x80);
            NEXT(3, 1)
        } else if (c < 0xf8) {
            unsigned char    c2, c3, c4;
            ucs4_t           code;

            RESERVE_INBUF(4)
            c2 = (*inbuf)[1]; c3 = (*inbuf)[2];
            c4 = (*inbuf)[3];
            if (!((c2 ^ 0x80) < 0x40 &&
                  (c3 ^ 0x80) < 0x40 && (c4 ^ 0x80) < 0x40 &&
                  (c >= 0xf1 || c2 >= 0x90)))
                return 4;
            code = ((ucs4_t)(c & 0x07) << 18)
                    | ((ucs4_t)(c2 ^ 0x80) << 12)
                    | ((ucs4_t)(c3 ^ 0x80) << 6)
                    | (ucs4_t)(c4 ^ 0x80);
            PUTUCS4(code)
            NEXT_IN(4)
        } else if (c < 0xfc) {
            unsigned char    c2, c3, c4, c5;
            ucs4_t           code;

            RESERVE_INBUF(5)
            c2 = (*inbuf)[1]; c3 = (*inbuf)[2];
            c4 = (*inbuf)[3]; c5 = (*inbuf)[4];
            if (!((c2 ^ 0x80) < 0x40 &&
                  (c3 ^ 0x80) < 0x40 && (c4 ^ 0x80) < 0x40 &&
                  (c5 ^ 0x80) < 0x40 && (c >= 0xf9 || c2 >= 0x88)))
                return 5;
            code = ((ucs4_t)(c & 0x03) << 24)
                    | ((ucs4_t)(c2 ^ 0x80) << 18)
                    | ((ucs4_t)(c3 ^ 0x80) << 12)
                    | ((ucs4_t)(c4 ^ 0x80) << 6)
                    | (ucs4_t)(c5 ^ 0x80);
            PUTUCS4(code)
            NEXT_IN(5)
        } else if (c < 0xff) {
            unsigned char    c2, c3, c4, c5, c6;
            ucs4_t           code;

            RESERVE_INBUF(6)
            c2 = (*inbuf)[1]; c3 = (*inbuf)[2];
            c4 = (*inbuf)[3]; c5 = (*inbuf)[4];
            c6 = (*inbuf)[5];
            if (!((c2 ^ 0x80) < 0x40 &&
                  (c3 ^ 0x80) < 0x40 && (c4 ^ 0x80) < 0x40 &&
                  (c5 ^ 0x80) < 0x40 && (c6 ^ 0x80) < 0x40 &&
                  (c >= 0xfd || c2 >= 0x84)))
                return 6;
            code = ((ucs4_t)(c & 0x01) << 30)
                    | ((ucs4_t)(c2 ^ 0x80) << 24)
                    | ((ucs4_t)(c3 ^ 0x80) << 18)
                    | ((ucs4_t)(c4 ^ 0x80) << 12)
                    | ((ucs4_t)(c5 ^ 0x80) << 6)
                    | (ucs4_t)(c6 ^ 0x80);
            PUTUCS4(code)
            NEXT_IN(6)
        } else
            return 1;
    }

    return 0;
}

#define CODEC_WITHOUT_MAPS
#include "codecentry.h"
CODEC_REGISTRY(utf_8)

/*
 * ex: ts=8 sts=4 et
 */
