/*
 * impl_euc_jisx0213.h: the EUC-JISX0213 codec implementation
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
 * $Id: impl_euc_jisx0213.h,v 1.1 2004/06/27 20:59:34 perky Exp $
 */

ENCODER(euc_jisx0213)
{
	while (inleft > 0) {
		ucs4_t c = IN1;
		DBCHAR code;
		int insize;

		if (c < 0x80) {
			WRITE1(c)
			NEXT(1, 1)
			continue;
		}

		DECODE_SURROGATE(c)
		insize = GET_INSIZE(c);

		if (c <= 0xFFFF) {
			/* try 0213 first because it might have MULTIC */
			TRYMAP_ENC(jisx0213_bmp, code, c) {
				if (code == MULTIC) {
					if (inleft < 2) {
						if (flags & MBENC_FLUSH) {
							code = find_pairencmap(
							    (ucs2_t)c, 0,
							   jisx0213_pairencmap,
							    JISX0213_ENCPAIRS);
							if (code == DBCINV)
								return 1;
						}
						else
							return MBERR_TOOFEW;
					}
					else {
						code = find_pairencmap(
							(ucs2_t)c, (*inbuf)[1],
							jisx0213_pairencmap,
							JISX0213_ENCPAIRS);
						if (code == DBCINV) {
							code = find_pairencmap(
							    (ucs2_t)c, 0,
							    jisx0213_pairencmap,
							    JISX0213_ENCPAIRS);
							if (code == DBCINV)
								return 1;
						} else
							insize = 2;
					}
				}
			}
			else TRYMAP_ENC(jisxcommon, code, c);
			else if (c >= 0xff61 && c <= 0xff9f) {
				/* JIS X 0201 half-width katakana */
				WRITE2(0x8e, c - 0xfec0)
				NEXT(1, 2)
				continue;
			}
			else if (c == 0xff3c)
				/* F/W REVERSE SOLIDUS (see NOTES) */
				code = 0x2140;
			else if (c == 0xff5e)
				/* F/W TILDE (see NOTES) */
				code = 0x2232;
			else
				return 1;
		}
		else if (c >> 16 == EMPBASE >> 16) {
			TRYMAP_ENC(jisx0213_emp, code, c & 0xffff);
			else return insize;
		}
		else
			return insize;

		if (code & 0x8000) {
			/* Codeset 2 */
			WRITE3(0x8f, code >> 8, (code & 0xFF) | 0x80)
			NEXT(insize, 3)
		} else {
			/* Codeset 1 */
			WRITE2((code >> 8) | 0x80, (code & 0xFF) | 0x80)
			NEXT(insize, 2)
		}
	}

	return 0;
}

DECODER(euc_jisx0213)
{
	while (inleft > 0) {
		unsigned char c = IN1;
		ucs4_t code;

		REQUIRE_OUTBUF(1)

		if (c < 0x80) {
			OUT1(c)
			NEXT(1, 1)
			continue;
		}

		if (c == 0x8e) {
			/* JIS X 0201 half-width katakana */
			unsigned char c2;

			REQUIRE_INBUF(2)
			c2 = IN2;
			if (c2 >= 0xa1 && c2 <= 0xdf) {
				OUT1(0xfec0 + c2)
				NEXT(2, 1)
			}
			else
				return 2;
		}
		else if (c == 0x8f) {
			unsigned char c2, c3;

			REQUIRE_INBUF(3)
			c2 = IN2 ^ 0x80;
			c3 = IN3 ^ 0x80;

			/* JIS X 0213 Plane 2 or JIS X 0212 (see NOTES) */
			TRYMAP_DEC(jisx0213_2_bmp, **outbuf, c2, c3) ;
			else TRYMAP_DEC(jisx0213_2_emp, code, c2, c3) {
				WRITEUCS4(EMPBASE | code)
				NEXT_IN(3)
				continue;
			}
			else TRYMAP_DEC(jisx0212, **outbuf, c2, c3) ;
			else return 3;
			NEXT(3, 1)
		}
		else {
			unsigned char c2;

			REQUIRE_INBUF(2)
			c ^= 0x80;
			c2 = IN2 ^ 0x80;

			/* JIS X 0213 Plane 1 */
			if (c == 0x21 && c2 == 0x40) **outbuf = 0xff3c;
			else if (c == 0x22 && c2 == 0x32) **outbuf = 0xff5e;
			else TRYMAP_DEC(jisx0208, **outbuf, c, c2);
			else TRYMAP_DEC(jisx0213_1_bmp, **outbuf, c, c2);
			else TRYMAP_DEC(jisx0213_1_emp, code, c, c2) {
				WRITEUCS4(EMPBASE | code)
				NEXT_IN(2)
				continue;
			}
			else TRYMAP_DEC(jisx0213_pair, code, c, c2) {
				WRITE2(code >> 16, code & 0xffff)
				NEXT(2, 2)
				continue;
			}
			else return 2;
			NEXT(2, 1)
		}
	}

	return 0;
}
