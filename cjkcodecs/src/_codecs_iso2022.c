/*
 * _codecs_iso2022.c: Codecs collection for ISO-2022 encodings.
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
 * $Id: _codecs_iso2022.c,v 1.1 2004/06/27 10:39:28 perky Exp $
 */

#include "cjkc_prelude.h"
#include "cjkc_interlude.h"

#define ESC			0x1B
#define SO			0x0E
#define SI			0x0F

#define CHARSET_ASCII		'B'
#define CHARSET_ISO8859_1	'A'
#define CHARSET_ISO8859_7	'F'
#define CHARSET_KSX1001		'C'
#define CHARSET_JISX0201_R	'J'
#define CHARSET_JISX0201_K	'I'
#define CHARSET_JISX0208	'B'
#define CHARSET_JISX0208_O	'@'
#define CHARSET_JISX0212	'D'
#define CHARSET_JISX0213_1	'O'
#define CHARSET_JISX0213_2	'P'
#define CHARSET_GB2312		'A'
#define CHARSET_GB2312_8565	'E'

#define MAP_UNMAPPABLE		0xFFFF
#define MAP_MULTIPLE_AVAIL	0xFFFE /* for JIS X 0213 */

#define F_SHIFTED		0x01
#define F_ESCTHROUGHOUT		0x02

#define STATE_SETG(dn, s, v)	((s)->c[dn]) = (v);
#define STATE_GETG(dn, s)	((s)->c[dn])

#define STATE_G0		STATE_GETG(0, state)
#define STATE_G1		STATE_GETG(1, state)
#define STATE_G2		STATE_GETG(2, state)
#define STATE_G3		STATE_GETG(3, state)
#define STATE_SETG0(v)		STATE_SETG(0, state, v)
#define STATE_SETG1(v)		STATE_SETG(1, state, v)
#define STATE_SETG2(v)		STATE_SETG(2, state, v)
#define STATE_SETG3(v)		STATE_SETG(3, state, v)

#define STATE_SETFLAG(f)	((state)->c[4]) |= (f);
#define STATE_GETFLAG(f)	((state)->c[4] & (f))
#define STATE_CLEARFLAG(f)	((state)->c[4]) &= ~(f);
#define STATE_CLEARFLAGS()	((state)->c[4]) = 0;

#define ISO2022_CONFIG		((const struct iso2022_config *)config)
#define CONFIG_ISSET(flag)	ISO2022_CONFIG |= (flag);
#define CONFIG_DESIGNATIONS	(ISO2022_CONFIG->designations)

/* iso2022_config.flags */
#define NO_SHIFT		(1<<0)
#define USE_G2			(1<<1)
#define USE_JISX0208_EXT	(1<<2)

/*-*- internal data structures -*-*/

typedef int (*iso2022_init_func)(void);
typedef ucs4_t (*iso2022_decode_func)(const char *data);
typedef DBCHAR (*iso2022_encode_func)(const ucs4_t *data, int length);

struct iso2022_designation {
	char mark;
	char plane;
	char width;
	iso2022_init_func initializer;
	iso2022_decode_func decoder;
	iso2022_encode_func encoder;
};

struct iso2022_config {
	int flags;
	const struct iso2022_designation designations[]; /* non-ascii desigs */
};

/*-*- iso-2022 codec implementation -*-*/

CODEC_INIT(iso2022)
{
	printf("iso2022 codec init\n");
	return 0;
}

ENCODER_INIT(iso2022)
{
	STATE_CLEARFLAGS()
	STATE_SETG0(CHARSET_ASCII)
	STATE_SETG1(CHARSET_ASCII)
	STATE_SETG2(CHARSET_ASCII)
	return 0;
}

ENCODER_RESET(iso2022)
{
	if (STATE_GETFLAG(F_SHIFTED)) {
		WRITE1(SI)
		NEXT_OUT(1)
		STATE_CLEARFLAG(F_SHIFTED)
	}
	return 0;
}

ENCODER(iso2022)
{
	while (inleft > 0) {
		const struct iso2022_designation *dsg;
		DBCHAR encoded;
		ucs4_t c = **inbuf;
		int insize;

		if (c < 0x80) {
			if (STATE_G0 != CHARSET_ASCII) {
				WRITE3(ESC, '(', 'B')
				STATE_SETG0(CHARSET_ASCII)
				NEXT_OUT(3)
			}
			if (STATE_GETFLAG(F_SHIFTED)) {
				WRITE1(SI)
				STATE_CLEARFLAG(F_SHIFTED)
				NEXT_OUT(1)
			}
			WRITE1((unsigned char)c)
			NEXT(1, 1)
			continue;
		}

		DECODE_SURROGATE(c)
		insize = GET_INSIZE(c);

		for (dsg = CONFIG_DESIGNATIONS; dsg->mark; dsg++) {
			encoded = dsg->encoder(&c, 1);
			if (encoded == MAP_UNMAPPABLE)
				continue;
		}

		if (!dsg->mark)
			return 1;
		assert(dsg->width == 1 || dsg->width == 2);

		switch (dsg->plane) {
		case 0: /* G0 */
			if (STATE_GETFLAG(F_SHIFTED)) {
				WRITE1(SO)
				STATE_CLEARFLAG(F_SHIFTED)
				NEXT_OUT(1)
			}
			if (STATE_G0 != dsg->mark) {
				if (dsg->width == 1) {
					WRITE3(ESC, '(', dsg->mark)
					STATE_SETG0(dsg->mark)
					NEXT_OUT(3)
				}
				else if (dsg->mark == CHARSET_JISX0208) {
					WRITE3(ESC, '$', dsg->mark)
					STATE_SETG0(dsg->mark)
					NEXT_OUT(3)
				}
				else {
					WRITE4(ESC, '$', '(', dsg->mark)
					STATE_SETG0(dsg->mark)
					NEXT_OUT(4)
				}
			}
			break;
		case 1: /* G1 */
			if (!STATE_GETFLAG(F_SHIFTED)) {
				WRITE1(SI)
				STATE_SETFLAG(F_SHIFTED)
				NEXT_OUT(1)
			}
			if (STATE_G1 != dsg->mark) {
				if (dsg->width == 1) {
					WRITE3(ESC, ')', dsg->mark)
					STATE_SETG1(dsg->mark)
					NEXT_OUT(3)
				}
				else {
					WRITE4(ESC, '$', ')', dsg->mark)
					STATE_SETG1(dsg->mark)
					NEXT_OUT(4)
				}
			}
			break;
		default: /* G2 and G3 is not supported: no encoding
			  * in CJKCodecs are using them yet */
			return MBERR_INTERNAL;
		}

		if (dsg->width == 1) {
			WRITE1((unsigned char)encoded)
		}
		else {
			WRITE2(encoded >> 8, encoded & 0xff)
		}
	}

	return 0;
}

DECODER_INIT(iso2022)
{
	return 0;
}

DECODER_RESET(iso2022)
{
	return 0;
}

DECODER(iso2022)
{
	return 0;
}

/*-*- mapping access functions -*-*/

int
ksx1001_init(void)
{
	return 0;
}

ucs4_t
ksx1001_decoder(const char *data)
{
	return 0;
}

DBCHAR
ksx1001_encoder(const ucs4_t *data, int length)
{
	return 0;
}

/*-*- registry tables -*-*/

static const struct iso2022_config iso2022_kr_config = {
	0,
	{{ CHARSET_KSX1001, 1, 2,
	   ksx1001_init, ksx1001_decoder, ksx1001_encoder },
	 { 0, }},
};

/*
struct iso2022_config iso2022_jp_3_config {
	ISO2022_NO_SHIFT | ISO2022_USE_G2 | ISO2022_USE_JISX0208_EXT,
	{{ CHARSET_ASCII, 0, 1, NULL, NULL },
	 { CHARSET_JISX0208, 0, 2, jisx0208_decoder, jisx0208_encoder },
	 { CHARSET_JISX0213_1, 0, 2, jisx0213_1_decoder, jisx0213_1_encoder },
	 { CHARSET_JISX0213_2, 0, 2, jisx0213_2_decoder, jisx0213_2_encoder }},
};
*/

BEGIN_MAPPING_LIST
  /* no mapping table here */
END_MAPPING_LIST

#define ISO2022_CODEC(variation) {		\
	"iso2022_" #variation,			\
	&iso2022_##variation##_config,		\
	iso2022_codec_init,			\
	_STATEFUL_METHODS(iso2022)		\
},

BEGIN_CODEC_LIST
  ISO2022_CODEC(kr)
END_CODEC_LIST

#include "cjkc_postlude.h"

I_AM_A_MODULE_FOR(iso2022)
