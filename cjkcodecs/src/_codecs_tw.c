/*
 * _codecs_tw.c: Codecs collection for Taiwan's encodings
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
 * $Id: _codecs_tw.c,v 1.9 2004/07/06 17:00:18 perky Exp $
 */

#include "cjkcodecs.h"
#include "map_big5.h"
#include "map_cp950ext.h"
#ifndef NO_EXTRA_ENCODINGS
# include "map_cns11643.h"
#endif

/*
 * BIG5 codec
 */

ENCODER(big5)
{
	while (inleft > 0) {
		Py_UNICODE c = **inbuf;
		DBCHAR code;

		if (c < 0x80) {
			REQUIRE_OUTBUF(1)
			**outbuf = (unsigned char)c;
			NEXT(1, 1)
			continue;
		}
		UCS4INVALID(c)

		REQUIRE_OUTBUF(2)

		TRYMAP_ENC(big5, code, c);
		else return 1;

		OUT1(code >> 8)
		OUT2(code & 0xFF)
		NEXT(1, 2)
	}

	return 0;
}

DECODER(big5)
{
	while (inleft > 0) {
		unsigned char c = IN1;

		REQUIRE_OUTBUF(1)

		if (c < 0x80) {
			OUT1(c)
			NEXT(1, 1)
			continue;
		}

		REQUIRE_INBUF(2)
		TRYMAP_DEC(big5, **outbuf, c, IN2) {
			NEXT(2, 1)
		}
		else return 2;
	}

	return 0;
}


/*
 * CP950 codec
 */

ENCODER(cp950)
{
	while (inleft > 0) {
		Py_UNICODE c = IN1;
		DBCHAR code;

		if (c < 0x80) {
			WRITE1((unsigned char)c)
			NEXT(1, 1)
			continue;
		}
		UCS4INVALID(c)

		REQUIRE_OUTBUF(2)
		TRYMAP_ENC(cp950ext, code, c);
		else TRYMAP_ENC(big5, code, c);
		else return 1;

		OUT1(code >> 8)
		OUT2(code & 0xFF)
		NEXT(1, 2)
	}

	return 0;
}

DECODER(cp950)
{
	while (inleft > 0) {
		unsigned char c = IN1;

		REQUIRE_OUTBUF(1)

		if (c < 0x80) {
			OUT1(c)
			NEXT(1, 1)
			continue;
		}

		REQUIRE_INBUF(2)

		TRYMAP_DEC(cp950ext, **outbuf, c, IN2);
		else TRYMAP_DEC(big5, **outbuf, c, IN2);
		else return 2;

		NEXT(2, 1)
	}

	return 0;
}

#ifndef NO_EXTRA_ENCODINGS
/*
 * EUC-TW codec
 */

ENCODER(euc_tw)
{
	while (inleft > 0) {
		ucs4_t c = IN1;
		unsigned char whi, wlo;
		int insize, plane;

		if (c < 0x80) {
			WRITE1((unsigned char)c)
			NEXT(1, 1)
			continue;
		}

		DECODE_SURROGATE(c)
		insize = GET_INSIZE(c);

		if (c <= 0xFFFF) {
			TRYMAP_ENC_MPLANE(cns11643_bmp, plane, whi, wlo, c);
			else return insize;
		}
		else if (0x20000 <= c && c < 0x30000) {
			TRYMAP_ENC_MPLANE(cns11643_nonbmp, plane, whi, wlo,
					  c & 0xffff);
			else return insize;
		}
		else
			return insize;

		if (plane == 1) {
			WRITE2(whi ^ 0x80, wlo ^ 0x80)
			NEXT(insize, 2)
		}
		else {
			WRITE4(0x8e, 0x80+plane,
				whi ^ 0x80, wlo ^ 0x80)
			NEXT(insize, 4)
		}
	}

	return 0;
}

DECODER(euc_tw)
{
	while (inleft > 0) {
		unsigned char c1 = IN1, c2;
		ucs4_t code;
		int plane, insize;

		REQUIRE_OUTBUF(1)
		if (c1 < 0x80) {
			OUT1(c1)
			NEXT(1, 1)
			continue;
		}

		if (c1 == 0x8e) {
			REQUIRE_INBUF(4)
			plane = IN2;
			if (plane < 0x81 || plane > 0x87)
				return 4;
			plane -= 0x80;
			insize = 4;
			c1 = IN3;
			c2 = IN4;
		}
		else {
			insize = 2;
			plane = 1;
			c2 = IN2;
		}

		TRYMAP_DEC_MPLANE(cns11643, code, plane,
				  c1 ^ 0x80, c2 ^ 0x80) { /* BMP */
			OUT1(code)
			NEXT(insize, 1)
		}
		else TRYMAP_DEC_MPLANE(cns11643, code, plane,
				       c1, c2 ^ 0x80) { /* non-BMP */
			WRITEUCS4(0x20000 | code)
			NEXT_IN(insize)
		}
		else
			return insize;
	}

	return 0;
}
#endif /* !ndef(NO_EXTRA_ENCODINGS) */


BEGIN_MAPPINGS_LIST
  MAPPING_ENCDEC(big5)
  MAPPING_ENCDEC(cp950ext)
#ifndef NO_EXTRA_ENCODINGS
  MAPPING_ENCONLY(cns11643_bmp)
  MAPPING_ENCONLY(cns11643_nonbmp)
  MAPPING_DECONLY(cns11643_1)
  MAPPING_DECONLY(cns11643_2)
  MAPPING_DECONLY(cns11643_3)
  MAPPING_DECONLY(cns11643_4)
  MAPPING_DECONLY(cns11643_5)
  MAPPING_DECONLY(cns11643_6)
  MAPPING_DECONLY(cns11643_7)
#endif
END_MAPPINGS_LIST

BEGIN_CODECS_LIST
  CODEC_STATELESS(big5)
  CODEC_STATELESS(cp950)
#ifndef NO_EXTRA_ENCODINGS
  CODEC_STATELESS(euc_tw)
#endif
END_CODECS_LIST

I_AM_A_MODULE_FOR(tw)
