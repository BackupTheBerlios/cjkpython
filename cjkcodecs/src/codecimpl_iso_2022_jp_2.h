/*
 * codecimpl_iso_2022_jp_2.c: the ISO-2022-JP-2 codec (RFC1554) implementation
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
 * $Id: codecimpl_iso_2022_jp_2.h,v 1.2 2004/06/18 18:15:24 perky Exp $
 */

#define ISO2022_ENCODING iso_2022_jp_2
#define ISO2022_DESIGNATIONS \
	CHARSET_ASCII, CHARSET_JISX0201_R, CHARSET_JISX0208, \
	CHARSET_JISX0208_O, CHARSET_JISX0212, CHARSET_GB2312, \
	CHARSET_KSX1001, CHARSET_JISX0212, \
	CHARSET_ISO8859_1, CHARSET_ISO8859_7
#define ISO2022_USE_G2_DESIGNATION  yo!
#define ISO2022_USE_JISX0208EXT

#include "iso2022sect.h"

ENCODER_INIT(iso_2022_jp_2)
{
	STATE_CLEARFLAGS(state)
	STATE_SETG0(state, CHARSET_ASCII)
	STATE_SETG1(state, CHARSET_ASCII)
	STATE_SETG2(state, CHARSET_ASCII)
	return 0;
}

ENCODER_RESET(iso_2022_jp_2)
{
	if (STATE_GETG0(state) != CHARSET_ASCII) {
		WRITE3(ESC, '(', 'B')
		STATE_SETG0(state, CHARSET_ASCII)
		NEXT_OUT(3)
	}
	return 0;
}

ENCODER(iso_2022_jp_2)
{
	while (inleft > 0) {
		Py_UNICODE c = IN1;
		DBCHAR code;

		if (c < 0x80) {
			switch (STATE_GETG0(state)) {
			case CHARSET_ASCII:
				WRITE1((unsigned char)c)
				NEXT(1, 1)
				break;
			case CHARSET_JISX0201_R:
				JISX0201_R_ENCODE(c, code)
				else { /* FALLTHROUGH (yay!) */
			default:	WRITE3(ESC, '(', 'B')
					NEXT_OUT(3)
					STATE_SETG0(state, CHARSET_ASCII)
					code = c;
				}
				WRITE1((unsigned char)code)
				NEXT(1, 1)
				break;
			}
			if (c == '\n')
				STATE_CLEARFLAG(state, F_SHIFTED)
		}
		else UCS4INVALID(c)
		else {
			unsigned char charset;

			charset = STATE_GETG0(state);
			if (charset == CHARSET_JISX0201_R) {
				code = DBCINV;
				JISX0201_R_ENCODE(c, code)
				if (code != DBCINV) {
					WRITE1((unsigned char)code)
					NEXT(1, 1)
					continue;
				}
			}

			TRYMAP_ENC(jisxcommon, code, c) {
				if (code & 0x8000) { /* MSB set: JIS X 0212 */
					if (charset != CHARSET_JISX0212) {
						WRITE4(ESC, '$', '(', 'D')
						STATE_SETG0(state,
							    CHARSET_JISX0212)
						NEXT_OUT(4)
					}
					WRITE2((code >> 8) & 0x7f, code & 0x7f)
				}
				else { /* MSB unset: JIS X 0208 */
jisx0208encode:				if (charset != CHARSET_JISX0208) {
						WRITE3(ESC, '$', 'B')
						STATE_SETG0(state,
							    CHARSET_JISX0208)
						NEXT_OUT(3)
					}
					WRITE2(code >> 8, code & 0xff)
				}
				NEXT(1, 2)
			}
			else TRYMAP_ENC(cp949, code, c) {
				if (code & 0x8000) /* MSB set: CP949 */
					return 2;
				if (charset != CHARSET_KSX1001) {
					WRITE4(ESC, '$', '(', 'C')
					STATE_SETG0(state, CHARSET_KSX1001)
					NEXT_OUT(4)
				}
				WRITE2(code >> 8, code & 0xff)
				NEXT(1, 2)
			}
			else TRYMAP_ENC(gbcommon, code, c) {
				if (code & 0x8000) /* MSB set: GBK */
					return 2;
				if (charset != CHARSET_GB2312) {
					WRITE4(ESC, '$', '(', 'A')
					STATE_SETG0(state, CHARSET_GB2312)
					NEXT_OUT(4)
				}
				WRITE2(code >> 8, code & 0xff)
				NEXT(1, 2)
			}
			else if (c == 0xff3c) {/* FULL-WIDTH REVERSE SOLIDUS */
				code = 0x2140;
				goto jisx0208encode;
			}
			else {
				JISX0201_R_ENCODE(c, code)
				else {
					/* There's no need to try to encode as
					 * ISO-8859-1 or ISO-8859-7 because JIS
					 * X 0212 includes them already. */
					return 1;
				}
				/* if (charset == CHARSET_JISX0201_R) :
				 * already checked */
				WRITE4(ESC, '(', 'J', (unsigned char)code)
				STATE_SETG0(state, CHARSET_JISX0201_R)
				NEXT(1, 4)
			}
		}
	}

	return 0;
}

DECODER_INIT(iso_2022_jp_2)
{
	STATE_CLEARFLAGS(state)
	STATE_SETG0(state, CHARSET_ASCII)
	STATE_SETG1(state, CHARSET_ASCII)
	STATE_SETG2(state, CHARSET_ASCII)
	return 0;
}

DECODER_RESET(iso_2022_jp_2)
{
	STATE_CLEARFLAG(state, F_SHIFTED)
	return 0;
}

DECODER(iso_2022_jp_2)
{
    BEGIN_ISO2022_LOOP

	unsigned char charset, c2;

	ISO2022_GETCHARSET(charset, c)

	if (charset & CHARSET_DOUBLEBYTE) {
		RESERVE_INBUF(2)
		RESERVE_OUTBUF(1)
		c2 = IN2;
		if (charset == CHARSET_JISX0208 ||
		    charset == CHARSET_JISX0208_O) {
			/* FULL-WIDTH REVERSE SOLIDUS */
			if (c == 0x21 && c2 == 0x40)
				**outbuf = 0xff3c;
			else TRYMAP_DEC(jisx0208, **outbuf, c, c2);
			else return 2;
		}
		else if (charset == CHARSET_JISX0212) {
			TRYMAP_DEC(jisx0212, **outbuf, c, c2);
			else return 2;
		}
		else if (charset == CHARSET_KSX1001) {
			TRYMAP_DEC(ksx1001, **outbuf, c, c2);
			else return 2;
		}
		else if (charset == CHARSET_GB2312) {
			TRYMAP_DEC(gb2312, **outbuf, c, c2);
			else return 2;
		}
		else
			return MBERR_INTERNAL;
		NEXT(2, 1)
	}
	else if (charset == CHARSET_ASCII) {
		RESERVE_OUTBUF(1)
		OUT1(c)
		NEXT(1, 1)
	}
	else if (charset == CHARSET_JISX0201_R) {
		RESERVE_OUTBUF(1)
		JISX0201_R_DECODE(c, **outbuf)
		else
			return 1;
		NEXT(1, 1)
	}
	else
		return MBERR_INTERNAL;

    END_ISO2022_LOOP
    return 0;
}

#include "iso2022sect.h"