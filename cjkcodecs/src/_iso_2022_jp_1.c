/*
 * _iso_2022_jp_1.c: the ISO-2022-JP-1 codec (RFC2237)
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
 * $Id: _iso_2022_jp_1.c,v 1.1 2003/09/24 17:44:48 perky Exp $
 */

#define ISO2022_DESIGNATIONS \
        CHARSET_ASCII, CHARSET_JISX0201_R, CHARSET_JISX0208, \
        CHARSET_JISX0208_O, CHARSET_JISX0212

#include "codeccommon.h"
#include "iso2022common.h"
#include "maps/alg_jisx0201.h"

ENCMAP(jisxcommon)
DECMAP(jisx0208)
DECMAP(jisx0212)

#define HAVE_ENCODER_INIT
ENCODER_INIT(iso_2022_jp_1)
{
    STATE_CLEARFLAGS(state)
    STATE_SETG0(state, CHARSET_ASCII)
    STATE_SETG1(state, CHARSET_ASCII)
    return 0;
}

#define HAVE_ENCODER_RESET
ENCODER_RESET(iso_2022_jp_1)
{
    if (STATE_GETG0(state) != CHARSET_ASCII) {
        RESERVE_OUTBUF(3)
        WRITE3(ESC, '(', 'B')
        STATE_SETG0(state, CHARSET_ASCII)
        NEXT_OUT(3)
    }
    return 0;
}

/* ISO-2022-JP-1 changes designations instead of shifting-out */

ENCODER(iso_2022_jp_1)
{
    while (inleft > 0) {
        Py_UNICODE  c = **inbuf;
        DBCHAR      code;

        if (c < 0x80) {
            switch (STATE_GETG0(state)) {
            case CHARSET_ASCII:
                WRITE1(c)
                NEXT(1, 1)
                break;
            case CHARSET_JISX0201_R:
                JISX0201_R_ENCODE(c, code)
                else { /* FALLTHROUGH (yay!) */
            default:
                    WRITE3(ESC, '(', 'B')
                    NEXT_OUT(3)
                    STATE_SETG0(state, CHARSET_ASCII)
                    code = c;
                }
                WRITE1(code)
                NEXT(1, 1)
                break;
            }
            if (c == '\n')
                STATE_CLEARFLAG(state, F_SHIFTED)
        } else UCS4INVALID(c)
        else {
            unsigned char    charset;

            charset = STATE_GETG0(state);
            if (charset == CHARSET_JISX0201_R) {
                code = DBCINV;
                JISX0201_R_ENCODE(c, code)
                if (code != DBCINV) {
                    WRITE1(code)
                    NEXT(1, 1)
                    continue;
                }
            }
            
            TRYMAP_ENC(jisxcommon, code, c) {
                if (code & 0x8000) { /* MSB set: JIS X 0212 */
                    if (charset != CHARSET_JISX0212) {
                        WRITE4(ESC, '$', '(', 'D')
                        STATE_SETG0(state, CHARSET_JISX0212)
                        NEXT_OUT(4)
                    }
                    WRITE2((code >> 8) & 0x7f, code & 0x7f)
                } else { /* MSB unset: JIS X 0208 */
                    if (charset != CHARSET_JISX0208) {
                        WRITE3(ESC, '$', 'B')
                        STATE_SETG0(state, CHARSET_JISX0208)
                        NEXT_OUT(3)
                    }
                    WRITE2(code >> 8, code & 0xff)
                }
                NEXT(1, 2)
            } else {
                JISX0201_R_ENCODE(c, code)
                else
                    return 1;
                /* if (charset == CHARSET_JISX0201_R) : already checked */
                WRITE4(ESC, '(', 'J', code)
                STATE_SETG0(state, CHARSET_JISX0201_R)
                NEXT(1, 4)
            }
        }
    }

    return 0;
}

#define HAVE_DECODER_INIT
DECODER_INIT(iso_2022_jp_1)
{
    STATE_CLEARFLAGS(state)
    STATE_SETG0(state, CHARSET_ASCII)
    STATE_SETG1(state, CHARSET_ASCII)
    return 0;
}

#define HAVE_DECODER_RESET
DECODER_RESET(iso_2022_jp_1)
{
    STATE_CLEARFLAG(state, F_SHIFTED)
    return 0;
}

DECODER(iso_2022_jp_1)
{
  ISO2022_LOOP_BEGIN
    unsigned char    charset, c2;

    ISO2022_GETCHARSET(charset, c, c2)

    if (charset & CHARSET_DOUBLEBYTE) {
        RESERVE_INBUF(2)
        RESERVE_OUTBUF(1)
        c2 &= IN2;
        if (charset == CHARSET_JISX0208 || charset == CHARSET_JISX0208_O) {
            TRYMAP_DEC(jisx0208, **outbuf, c, c2);
            else return 2;
        } else if (charset == CHARSET_JISX0212) {
            TRYMAP_DEC(jisx0212, **outbuf, c, c2);
            else return 2;
        } else
            return MBERR_INTERNAL;
        NEXT(2, 1)
    } else if (charset == CHARSET_ASCII) {
        RESERVE_OUTBUF(1)
        OUT1(c)
        NEXT(1, 1)
    } else if (charset == CHARSET_JISX0201_R) {
        RESERVE_OUTBUF(1)
        JISX0201_R_DECODE(c, **outbuf)
        else
            return 1;
        NEXT(1, 1)
    } else
        return MBERR_INTERNAL;
  ISO2022_LOOP_END

  return 0;
}

#include "codecentry.h"
BEGIN_CODEC_REGISTRY(iso_2022_jp_1)
    MAPOPEN(ja_JP)
        IMPORTMAP_DEC(jisx0208)
        IMPORTMAP_DEC(jisx0212)
        IMPORTMAP_ENC(jisxcommon)
    MAPCLOSE()
END_CODEC_REGISTRY(iso_2022_jp_1)

/*
 * ex: ts=8 sts=4 et
 */
