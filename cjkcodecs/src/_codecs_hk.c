/*
 * _codecs_hk.c: Codecs collection for encodings from Hong Kong
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
 * $Id: _codecs_hk.c,v 1.1 2004/06/28 18:16:03 perky Exp $
 */

#define USING_IMPORTED_MAPS

#include "cjkcodecs.h"
#include "map_big5hkscs.h"

/*
 * BIG5HKSCS codec
 */

static const encode_map *big5_encmap = NULL;
static const decode_map *big5_decmap = NULL;

CODEC_INIT(big5hkscs)
{
	static int initialized = 0;

	if (!initialized && IMPORT_MAP(tw, big5, &big5_encmap, &big5_decmap))
		return -1;
	initialized = 1;
	return 0;
}

ENCODER(big5hkscs)
{
	while (inleft > 0) {
		ucs4_t c = **inbuf;
		DBCHAR code;
		int insize;

		if (c < 0x80) {
			REQUIRE_OUTBUF(1)
			**outbuf = (unsigned char)c;
			NEXT(1, 1)
			continue;
		}

		DECODE_SURROGATE(c)
		insize = GET_INSIZE(c);

		REQUIRE_OUTBUF(2)

		if (c < 0x10000) {
			TRYMAP_ENC(big5, code, c);
			else TRYMAP_ENC(big5hkscs_bmp, code, c);
			else return 1;
		}
		else if (c < 0x20000)
			return insize;
		else if (c < 0x30000) {
			TRYMAP_ENC(big5hkscs_nonbmp, code, c & 0xffff);
			else return insize;
		}
		else
			return insize;

		OUT1(code >> 8)
		OUT2(code & 0xFF)
		NEXT(1, 2)
	}

	return 0;
}

#define BH2S(c1, c2) (((c1) - 0x88) * (0xfe - 0x40 + 1) + ((c2) - 0x40))

DECODER(big5hkscs)
{
	while (inleft > 0) {
		unsigned char c = IN1;
		ucs4_t decoded;

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
		else TRYMAP_DEC(big5hkscs, decoded, c, IN2) {
			int s = BH2S(c, IN2);
			const unsigned char *hintbase;

			assert(0x88 <= c && c <= 0xfe);
			assert(0x40 <= IN2 && IN2 <= 0xfe);

			if (BH2S(0x88, 0x40) <= s && s <= BH2S(0xa0, 0xfe)) {
				hintbase = big5hkscs_phint_0;
				s -= BH2S(0x88, 0x40);
			}
			else if (BH2S(0xc6,0xa1) <= s && s <= BH2S(0xc8,0xfe)){
				hintbase = big5hkscs_phint_11939;
				s -= BH2S(0xc6, 0xa1);
			}
			else if (BH2S(0xf9,0xd6) <= s && s <= BH2S(0xfe,0xfe)){
				hintbase = big5hkscs_phint_21733;
				s -= BH2S(0xf9, 0xd6);
			}
			else
				return MBERR_INTERNAL;

			if (hintbase[s >> 3] & (1 << (s & 7))) {
				WRITEUCS4(decoded | 0x20000)
				NEXT_IN(2)
			}
			else {
				OUT1(decoded)
				NEXT(2, 1)
			}
		}
		else return 2;
	}

	return 0;
}


BEGIN_MAPPINGS_LIST
  MAPPING_DECONLY(big5hkscs)
  MAPPING_ENCONLY(big5hkscs_bmp)
  MAPPING_ENCONLY(big5hkscs_nonbmp)
END_MAPPINGS_LIST

BEGIN_CODECS_LIST
  CODEC_STATELESS_WINIT(big5hkscs)
END_CODECS_LIST

I_AM_A_MODULE_FOR(hk)
