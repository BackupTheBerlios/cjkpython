/*
 * impl_cp932.h: the CP932 codec implementation
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
 * $Id: impl_cp932.h,v 1.1 2004/06/27 20:59:34 perky Exp $
 */

ENCODER(cp932)
{
	while (inleft > 0) {
		Py_UNICODE c = IN1;
		DBCHAR code;
		unsigned char c1, c2;

		if (c <= 0x80) {
			WRITE1((unsigned char)c)
			NEXT(1, 1)
			continue;
		}
		else if (c >= 0xff61 && c <= 0xff9f) {
			WRITE1(c - 0xfec0)
			NEXT(1, 1)
			continue;
		}
		else if (c >= 0xf8f0 && c <= 0xf8f3) {
			/* Windows compatability */
			REQUIRE_OUTBUF(1)
			if (c == 0xf8f0)
				OUT1(0xa0)
			else
				OUT1(c - 0xfef1 + 0xfd)
			NEXT(1, 1)
			continue;
		}

		UCS4INVALID(c)
		REQUIRE_OUTBUF(2)

		TRYMAP_ENC(cp932ext, code, c) {
			OUT1(code >> 8)
			OUT2(code & 0xff)
		}
		else TRYMAP_ENC(jisxcommon, code, c) {
			if (code & 0x8000) /* MSB set: JIS X 0212 */
				return 1;

			/* JIS X 0208 */
			c1 = code >> 8;
			c2 = code & 0xff;
			c2 = (((c1 - 0x21) & 1) ? 0x5e : 0) + (c2 - 0x21);
			c1 = (c1 - 0x21) >> 1;
			OUT1(c1 < 0x1f ? c1 + 0x81 : c1 + 0xc1)
			OUT2(c2 < 0x3f ? c2 + 0x40 : c2 + 0x41)
		}
		else if (c >= 0xe000 && c < 0xe758) {
			/* User-defined area */
			c1 = (Py_UNICODE)(c - 0xe000) / 188;
			c2 = (Py_UNICODE)(c - 0xe000) % 188;
			OUT1(c1 + 0xf0)
			OUT2(c2 < 0x3f ? c2 + 0x40 : c2 + 0x41)
		}
		else
			return 1;

		NEXT(1, 2)
	}

	return 0;
}

DECODER(cp932)
{
	while (inleft > 0) {
		unsigned char c = IN1, c2;

		REQUIRE_OUTBUF(1)
		if (c <= 0x80) {
			OUT1(c)
			NEXT(1, 1)
			continue;
		}
		else if (c >= 0xa0 && c <= 0xdf) {
			if (c == 0xa0)
				OUT1(0xf8f0) /* half-width katakana */
			else
				OUT1(0xfec0 + c)
			NEXT(1, 1)
			continue;
		}
		else if (c >= 0xfd/* && c <= 0xff*/) {
			/* Windows compatibility */
			OUT1(0xf8f1 - 0xfd + c)
			NEXT(1, 1)
			continue;
		}

		REQUIRE_INBUF(2)
		c2 = IN2;

		TRYMAP_DEC(cp932ext, **outbuf, c, c2);
		else if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xea)){
			if (c2 < 0x40 || (c2 > 0x7e && c2 < 0x80) || c2 > 0xfc)
				return 2;

			c = (c < 0xe0 ? c - 0x81 : c - 0xc1);
			c2 = (c2 < 0x80 ? c2 - 0x40 : c2 - 0x41);
			c = (2 * c + (c2 < 0x5e ? 0 : 1) + 0x21);
			c2 = (c2 < 0x5e ? c2 : c2 - 0x5e) + 0x21;

			TRYMAP_DEC(jisx0208, **outbuf, c, c2);
			else return 2;
		}
		else if (c >= 0xf0 && c <= 0xf9) {
			if ((c2 >= 0x40 && c2 <= 0x7e) ||
			    (c2 >= 0x80 && c2 <= 0xfc))
				OUT1(0xe000 + 188 * (c - 0xf0) +
				     (c2 < 0x80 ? c2 - 0x40 : c2 - 0x41))
			else
				return 2;
		}
		else
			return 2;

		NEXT(2, 1)
	}

	return 0;
}
