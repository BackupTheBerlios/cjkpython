/*
 * _iso_2022_kr.c: the ISO-2022-KR codec (RFC1557)
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
 * $Id: _iso_2022_kr.c,v 1.4 2004/03/10 07:44:09 perky Exp $
 */

#define ISO2022_DESIGNATIONS \
        CHARSET_ASCII, CHARSET_KSX1001

#include "codeccommon.h"
#include "iso2022common.h"

ENCMAP(cp949)
DECMAP(ksx1001)

#define HAVE_ENCODER_INIT
ENCODER_INIT(iso_2022_kr)
{
    STATE_CLEARFLAGS(state)
    STATE_SETG0(state, CHARSET_ASCII)
    STATE_SETG1(state, CHARSET_ASCII)
    return 0;
}

#define HAVE_ENCODER_RESET
ENCODER_RESET(iso_2022_kr)
{
    if (STATE_GETFLAG(state, F_SHIFTED)) {
        RESERVE_OUTBUF(1)
        OUT1(SI)
        NEXT_OUT(1)
        STATE_CLEARFLAG(state, F_SHIFTED)
    }
    return 0;
}

ENCODER(iso_2022_kr)
{
    while (inleft > 0) {
        Py_UNICODE  c = **inbuf;
        DBCHAR      code;

        if (c < 0x80) {
            if (STATE_GETFLAG(state, F_SHIFTED)) {
                WRITE2(SI, (unsigned char)c)
                STATE_CLEARFLAG(state, F_SHIFTED)
                NEXT(1, 2)
            } else {
                WRITE1((unsigned char)c)
                NEXT(1, 1)
            }
            if (c == '\n')
                STATE_CLEARFLAG(state, F_SHIFTED)
        } else UCS4INVALID(c)
        else {
            if (STATE_GETG1(state) != CHARSET_KSX1001) {
                WRITE4(ESC, '$', ')', 'C')
                STATE_SETG1(state, CHARSET_KSX1001)
                NEXT_OUT(4)
            }

            if (!STATE_GETFLAG(state, F_SHIFTED)) {
                WRITE1(SO)
                STATE_SETFLAG(state, F_SHIFTED)
                NEXT_OUT(1)
            }

            TRYMAP_ENC(cp949, code, c) {
                if (code & 0x8000) /* MSB set: CP949 */
                    return 1;
                WRITE2(code >> 8, code & 0xff)
                NEXT(1, 2)
            } else
                return 1;
        }
    }

    return 0;
}

#define HAVE_DECODER_INIT
DECODER_INIT(iso_2022_kr)
{
    STATE_CLEARFLAGS(state)
    STATE_SETG0(state, CHARSET_ASCII)
    STATE_SETG1(state, CHARSET_ASCII)
    return 0;
}

#define HAVE_DECODER_RESET
DECODER_RESET(iso_2022_kr)
{
    STATE_CLEARFLAG(state, F_SHIFTED)
    return 0;
}

DECODER(iso_2022_kr)
{
  ISO2022_LOOP_BEGIN
    unsigned char    charset, c2;

    ISO2022_GETCHARSET(charset, c)

    if (charset & CHARSET_DOUBLEBYTE) {
        /* all double byte character sets are in KS X 1001 here */
        RESERVE_INBUF(2)
        RESERVE_OUTBUF(1)
        c2 = IN2;
        if (c2 >= 0x80)
            return 1;
        TRYMAP_DEC(ksx1001, **outbuf, c, c2) {
            NEXT(2, 1)
        } else
            return 2;
    } else {
        RESERVE_OUTBUF(1)
        OUT1(c);
        NEXT(1, 1)
    }
  ISO2022_LOOP_END
  return 0;
}

#include "codecentry.h"
BEGIN_CODEC_REGISTRY(iso_2022_kr)
    MAPOPEN(ko_KR)
        IMPORTMAP_DEC(ksx1001)
        IMPORTMAP_ENC(cp949)
    MAPCLOSE()
END_CODEC_REGISTRY(iso_2022_kr)

/*
 * ex: ts=8 sts=4 et
 */
