/*
 * _shift_jis.c: the SHIFT-JIS codec
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
 * $Id: _shift_jis.c,v 1.1 2003/09/24 17:44:49 perky Exp $
 */

#include "codeccommon.h"
#include "maps/alg_jisx0201.h"

ENCMAP(jisxcommon)
DECMAP(jisx0208)

ENCODER(shift_jis)
{
    while (inleft > 0) {
        Py_UNICODE       c = IN1;
        DBCHAR           code;
        unsigned char    c1, c2;

        JISX0201_ENCODE(c, code)
        else UCS4INVALID(c)
        else code = NOCHAR;

        if (code < 0x80 || (code >= 0xa1 && code <= 0xdf)) {
            RESERVE_OUTBUF(1)

            OUT1(code)
            NEXT(1, 1)
            continue;
        }

        RESERVE_OUTBUF(2)

        if (code == NOCHAR) {
            TRYMAP_ENC(jisxcommon, code, c);
            else if (c >= 0xe000 && c < 0xe758) {
                /* user-defined area */
                c1 = (Py_UNICODE)(c - 0xe000) / 188;
                c2 = (Py_UNICODE)(c - 0xe000) % 188;
                OUT1(c1 + 0xf0)
                OUT2(c2 < 0x3f ? c2 + 0x40 : c2 + 0x41)
                NEXT(1, 2)
                continue;
            } else
                return 1;

            if (code & 0x8000) /* MSB set: JIS X 0212 */
                return 1;
        }

        c1 = code >> 8;
        c2 = code & 0xff;
        c2 = (((c1 - 0x21) & 1) ? 0x5e : 0) + (c2 - 0x21);
        c1 = (c1 - 0x21) >> 1;
        OUT1(c1 < 0x1f ? c1 + 0x81 : c1 + 0xc1)
        OUT2(c2 < 0x3f ? c2 + 0x40 : c2 + 0x41)
        NEXT(1, 2)
    }

    return 0;
}

DECODER(shift_jis)
{
    while (inleft > 0) {
        unsigned char    c = IN1;

        RESERVE_OUTBUF(1)
        JISX0201_DECODE(c, **outbuf)
        else if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xea)) {
            unsigned char    c1, c2;

            RESERVE_INBUF(2)
            c2 = IN2;
            if (c2 < 0x40 || (c2 > 0x7e && c2 < 0x80) || c2 > 0xfc)
                return 2;

            c1 = (c < 0xe0 ? c - 0x81 : c - 0xc1);
            c2 = (c2 < 0x80 ? c2 - 0x40 : c2 - 0x41);
            c1 = (2 * c1 + (c2 < 0x5e ? 0 : 1) + 0x21);
            c2 = (c2 < 0x5e ? c2 : c2 - 0x5e) + 0x21;

            TRYMAP_DEC(jisx0208, **outbuf, c1, c2) {
                NEXT(2, 1)
                continue;
            } else
                return 2;
        } else if (c >= 0xf0 && c <= 0xf9) {
            unsigned char    c2;

            RESERVE_INBUF(2)
            c2 = IN2;
            if ((c2 >= 0x40 && c2 <= 0x7e) || (c2 >= 0x80 && c2 <= 0xfc)) {
                OUT1(0xe000 + 188 * (c - 0xf0) +
                       (c2 < 0x80 ? c2 - 0x40 : c2 - 0x41))
                NEXT(2, 1)
                continue;
            } else
                return 2;
        } else
            return 2;

        NEXT(1, 1) /* JIS X 0201 */
    }

    return 0;
}

#include "codecentry.h"
BEGIN_CODEC_REGISTRY(shift_jis)
    MAPOPEN(ja_JP)
        IMPORTMAP_DEC(jisx0208)
        IMPORTMAP_ENC(jisxcommon)
    MAPCLOSE()
END_CODEC_REGISTRY(shift_jis)

/*
 * ex: ts=8 sts=4 et
 */
