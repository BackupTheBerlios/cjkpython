/*
 * _gb2312.c: the GB2312 codec
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
 * $Id: _gb2312.c,v 1.2 2003/12/31 05:46:55 perky Exp $
 */

#include "codeccommon.h"

ENCMAP(gbcommon)
DECMAP(gb2312)

ENCODER(gb2312)
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

        RESERVE_OUTBUF(2)
        TRYMAP_ENC(gbcommon, code, c);
        else return 1;

        if (code & 0x8000) /* MSB set: GBK */
            return 1;

        OUT1((code >> 8) | 0x80)
        OUT2((code & 0xFF) | 0x80)
        NEXT(1, 2)
    }

    return 0;
}

DECODER(gb2312)
{
    while (inleft > 0) {
        unsigned char    c = **inbuf;

        RESERVE_OUTBUF(1)

        if (c < 0x80) {
            OUT1(c)
            NEXT(1, 1)
            continue;
        }

        RESERVE_INBUF(2)
        TRYMAP_DEC(gb2312, **outbuf, c ^ 0x80, IN2 ^ 0x80) {
            NEXT(2, 1)
        } else return 2;
    }

    return 0;
}

#include "codecentry.h"
BEGIN_CODEC_REGISTRY(gb2312)
    MAPOPEN(zh_CN)
        IMPORTMAP_DEC(gb2312)
        IMPORTMAP_ENC(gbcommon)
    MAPCLOSE()
END_CODEC_REGISTRY(gb2312)

/*
 * ex: ts=8 sts=4 et
 */
