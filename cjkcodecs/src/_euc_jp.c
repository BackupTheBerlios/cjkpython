/*
 * _euc_jp.c: the EUC-JP codec
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
 * $Id: _euc_jp.c,v 1.4 2003/12/30 03:00:40 perky Exp $
 */

#include "codeccommon.h"

ENCMAP(jisxcommon)
DECMAP(jisx0208)
DECMAP(jisx0212)

ENCODER(euc_jp)
{
    while (inleft > 0) {
        Py_UNICODE  c = IN1;
        DBCHAR      code;

        if (c < 0x80) {
            WRITE1(c)
            NEXT(1, 1)
            continue;
        }

        UCS4INVALID(c)

        TRYMAP_ENC(jisxcommon, code, c);
        else if (c >= 0xff61 && c <= 0xff9f) {
            /* JIS X 0201 half-width katakana */
            WRITE2(0x8e, c - 0xfec0)
            NEXT(1, 2)
            continue;
        }
#ifndef STRICT_BUILD
        else if (c == 0xff3c) /* FULL-WIDTH REVERSE SOLIDUS */
            code = 0x2140;
        else if (c == 0xa5) { /* YEN SIGN */
            WRITE1(0x5c);
            NEXT(1, 1)
            continue;
        } else if (c == 0x203e) { /* OVERLINE */
            WRITE1(0x7e);
            NEXT(1, 1)
            continue;
        }
#endif
        else
            return 1;

        if (code & 0x8000) {
            /* JIS X 0212 */
            WRITE3(0x8f, code >> 8, (code & 0xFF) | 0x80)
            NEXT(1, 3)
        } else {
            /* JIS X 0208 */
            WRITE2((code >> 8) | 0x80, (code & 0xFF) | 0x80)
            NEXT(1, 2)
        }
    }

    return 0;
}

DECODER(euc_jp)
{
    while (inleft > 0) {
        unsigned char    c = IN1;

        RESERVE_OUTBUF(1)

        if (c < 0x80) {
            OUT1(c)
            NEXT(1, 1)
            continue;
        }

        if (c == 0x8e) {
            /* JIS X 0201 half-width katakana */
            unsigned char    c2;

            RESERVE_INBUF(2)
            c2 = IN2;
            if (c2 >= 0xa1 && c2 <= 0xdf) {
                OUT1(0xfec0 + c2)
                NEXT(2, 1)
            } else
                return 2;
        } else if (c == 0x8f) {
            unsigned char    c2, c3;

            RESERVE_INBUF(3)
            c2 = IN2;
            c3 = IN3;
            /* JIS X 0212 */
            TRYMAP_DEC(jisx0212, **outbuf, c2 ^ 0x80, c3 ^ 0x80) {
                NEXT(3, 1)
            } else
                return 3;
        } else {
            unsigned char    c2;

            RESERVE_INBUF(2)
            c2 = IN2;
            /* JIS X 0208 */
#ifndef STRICT_BUILD
            if (c == 0xa1 && c2 == 0xc0) /* FULL-WIDTH REVERSE SOLIDUS */
                **outbuf = 0xff3c;
            else
#endif
            TRYMAP_DEC(jisx0208, **outbuf, c ^ 0x80, c2 ^ 0x80) ;
            else return 2;
            NEXT(2, 1)
        }
    }

    return 0;
}

#include "codecentry.h"
BEGIN_CODEC_REGISTRY(euc_jp)
    MAPOPEN(ja_JP)
        IMPORTMAP_DEC(jisx0208)
        IMPORTMAP_DEC(jisx0212)
        IMPORTMAP_ENC(jisxcommon)
    MAPCLOSE()
END_CODEC_REGISTRY(euc_jp)

/*
 * ex: ts=8 sts=4 et
 */
