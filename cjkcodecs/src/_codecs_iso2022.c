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
 * $Id: _codecs_iso2022.c,v 1.2 2004/06/27 19:17:47 perky Exp $
 */

#define USING_IMPORTED_MAPS
#include "cjkc_prelude.h"
#include "maps/alg_iso8859_1.h"
#include "maps/alg_iso8859_7.h"
#include "cjkc_interlude.h"

/* STATE

   state->c[0-3]

	00000000
	||^^^^^|
	|+-----+----  G0-3 Character Set
	+-----------  Is G0-3 double byte?

   state->c[4]

	00000000
	      ||
	      |+----  Locked-Shift?
	      +-----  ESC Throughout
*/

#define ESC			0x1B
#define SO			0x0E
#define SI			0x0F
#define LF			0x0A

#define MAX_ESCSEQLEN		16

#define CHARSET_ASCII		'B'
#define CHARSET_ISO8859_1	'A'
#define CHARSET_ISO8859_7	'F'
#define CHARSET_KSX1001		('C'|CHARSET_DBCS)
#define CHARSET_JISX0201_R	('J'|CHARSET_DBCS)
#define CHARSET_JISX0201_K	('I'|CHARSET_DBCS)
#define CHARSET_JISX0208	('B'|CHARSET_DBCS)
#define CHARSET_JISX0208_O	('@'|CHARSET_DBCS)
#define CHARSET_JISX0212	('D'|CHARSET_DBCS)
#define CHARSET_JISX0213_1	('O'|CHARSET_DBCS)
#define CHARSET_JISX0213_2	('P'|CHARSET_DBCS)
#define CHARSET_GB2312		('A'|CHARSET_DBCS)
#define CHARSET_GB2312_8565	('E'|CHARSET_DBCS)

#define CHARSET_DBCS		0x80
#define ESCMARK(mark)		((mark) & 0x7f)

#define IS_ESCEND(c)	(((c) >= 'A' && (c) <= 'Z') || (c) == '@')
#define IS_ISO2022ESC(c2) \
		((c2) == '(' || (c2) == ')' || (c2) == '$' || \
		 (c2) == '.' || (c2) == '&')
	/* this is not a complete list of ISO-2022 escape sequence headers.
	 * but, it's enough to implement CJK instances of iso-2022. */

#define MAP_UNMAPPABLE		0xFFFF
#define MAP_MULTIPLE_AVAIL	0xFFFE /* for JIS X 0213 */

#define F_SHIFTED		0x01
#define F_ESCTHROUGHOUT		0x02

#define STATE_SETG(dn, v)	((state)->c[dn]) = (v);
#define STATE_GETG(dn)		((state)->c[dn])

#define STATE_G0		STATE_GETG(0)
#define STATE_G1		STATE_GETG(1)
#define STATE_G2		STATE_GETG(2)
#define STATE_G3		STATE_GETG(3)
#define STATE_SETG0(v)		STATE_SETG(0, v)
#define STATE_SETG1(v)		STATE_SETG(1, v)
#define STATE_SETG2(v)		STATE_SETG(2, v)
#define STATE_SETG3(v)		STATE_SETG(3, v)

#define STATE_SETFLAG(f)	((state)->c[4]) |= (f);
#define STATE_GETFLAG(f)	((state)->c[4] & (f))
#define STATE_CLEARFLAG(f)	((state)->c[4]) &= ~(f);
#define STATE_CLEARFLAGS()	((state)->c[4]) = 0;

#define ISO2022_CONFIG		((const struct iso2022_config *)config)
#define CONFIG_ISSET(flag)	(ISO2022_CONFIG->flags & (flag))
#define CONFIG_DESIGNATIONS	(ISO2022_CONFIG->designations)

/* iso2022_config.flags */
#define NO_SHIFT		0x01
#define USE_G2			0x02
#define USE_JISX0208_EXT	0x04

/*-*- internal data structures -*-*/

typedef int (*iso2022_init_func)(void);
typedef ucs4_t (*iso2022_decode_func)(const unsigned char *data);
typedef DBCHAR (*iso2022_encode_func)(const ucs4_t *data, int length);

struct iso2022_designation {
	unsigned char mark;
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
	const struct iso2022_designation *desig = CONFIG_DESIGNATIONS;
	for (desig = CONFIG_DESIGNATIONS; desig->mark; desig++)
		if (desig->initializer() != 0)
			return -1;
	return 0;
}

ENCODER_INIT(iso2022)
{
	STATE_CLEARFLAGS()
	STATE_SETG0(CHARSET_ASCII)
	STATE_SETG1(CHARSET_ASCII)
	return 0;
}

ENCODER_RESET(iso2022)
{
	if (STATE_GETFLAG(F_SHIFTED)) {
		WRITE1(SI)
		NEXT_OUT(1)
		STATE_CLEARFLAG(F_SHIFTED)
	}
	if (STATE_G0 != CHARSET_ASCII) {
		WRITE3(ESC, '(', 'B')
		NEXT_OUT(3)
		STATE_SETG0(CHARSET_ASCII)
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
			if (encoded != MAP_UNMAPPABLE)
				break;
		}

		if (!dsg->mark)
			return 1;
		assert(dsg->width == 1 || dsg->width == 2);

		switch (dsg->plane) {
		case 0: /* G0 */
			if (STATE_GETFLAG(F_SHIFTED)) {
				WRITE1(SI)
				STATE_CLEARFLAG(F_SHIFTED)
				NEXT_OUT(1)
			}
			if (STATE_G0 != dsg->mark) {
				if (dsg->width == 1) {
					WRITE3(ESC, '(', ESCMARK(dsg->mark))
					STATE_SETG0(dsg->mark)
					NEXT_OUT(3)
				}
				else if (dsg->mark == CHARSET_JISX0208) {
					WRITE3(ESC, '$', ESCMARK(dsg->mark))
					STATE_SETG0(dsg->mark)
					NEXT_OUT(3)
				}
				else {
					WRITE4(ESC, '$', '(',
						ESCMARK(dsg->mark))
					STATE_SETG0(dsg->mark)
					NEXT_OUT(4)
				}
			}
			break;
		case 1: /* G1 */
			if (STATE_G1 != dsg->mark) {
				if (dsg->width == 1) {
					WRITE3(ESC, ')', ESCMARK(dsg->mark))
					STATE_SETG1(dsg->mark)
					NEXT_OUT(3)
				}
				else {
					WRITE4(ESC, '$', ')',
						ESCMARK(dsg->mark))
					STATE_SETG1(dsg->mark)
					NEXT_OUT(4)
				}
			}
			if (!STATE_GETFLAG(F_SHIFTED)) {
				WRITE1(SO)
				STATE_SETFLAG(F_SHIFTED)
				NEXT_OUT(1)
			}
			break;
		default: /* G2 and G3 is not supported: no encoding in
			  * CJKCodecs are using them yet */
			return MBERR_INTERNAL;
		}

		if (dsg->width == 1) {
			WRITE1((unsigned char)encoded)
			NEXT_OUT(1)
		}
		else {
			WRITE2(encoded >> 8, encoded & 0xff)
			NEXT_OUT(2)
		}
		NEXT_IN(insize)
	}

	return 0;
}

DECODER_INIT(iso2022)
{
	STATE_CLEARFLAGS()
	STATE_SETG0(CHARSET_ASCII)
	STATE_SETG1(CHARSET_ASCII)
	STATE_SETG2(CHARSET_ASCII)
	return 0;
}

DECODER_RESET(iso2022)
{
	STATE_SETG0(CHARSET_ASCII)
	STATE_CLEARFLAG(F_SHIFTED)
	return 0;
}

static int
iso2022processesc(const void *config, MultibyteCodec_State *state,
		  const unsigned char **inbuf, size_t *inleft)
{
	unsigned char charset, designation;
	size_t i, esclen;

	for (i = 1;i < MAX_ESCSEQLEN;i++) {
		if (i >= *inleft)
			return MBERR_TOOFEW;
		if (IS_ESCEND((*inbuf)[i])) {
			esclen = i + 1;
			break;
		}
		else if (CONFIG_ISSET(USE_JISX0208_EXT) && i+1 < *inleft &&
			 (*inbuf)[i] == '&' && (*inbuf)[i+1] == '@')
			i += 2;
	}

	if (i >= MAX_ESCSEQLEN)
		return 1; /* unterminated escape sequence */

	switch (esclen) {
	case 3:
		if (IN2 == '$') {
			charset = IN3 | CHARSET_DBCS;
			designation = 0;
		}
		else {
			charset = IN3;
			if (IN2 == '(') designation = 0;
			else if (IN2 == ')') designation = 1;
			else if (CONFIG_ISSET(USE_G2) && IN2 == '.')
				designation = 2;
			else return 3;
		}
		break;
	case 4:
		if (IN2 != '$')
			return 4;

		charset = IN4 | CHARSET_DBCS;
		if (IN3 == '(') designation = 0;
		else if (IN3 == ')') designation = 1;
		else return 4;
		break;
	case 6: /* designation with prefix */
		if (CONFIG_ISSET(USE_JISX0208_EXT) &&
		    (*inbuf)[3] == ESC && (*inbuf)[4] == '$' &&
		    (*inbuf)[5] == 'B') {
			charset = 'B' | CHARSET_DBCS;
			designation = 0;
		}
		else
			return 6;
		break;
	default:
		return esclen;
	}

	{/* raise error when the charset is not designated for this encoding */
		const struct iso2022_designation *dsg;

		for (dsg = CONFIG_DESIGNATIONS; dsg->mark; dsg++)
			if (dsg->mark == charset)
				break;
		if (!dsg->mark)
			return esclen;
	}

	STATE_SETG(designation, charset)
	*inleft -= esclen;
	(*inbuf) += esclen;
	return 0;
}

static int
iso2022processg2(const void *config, MultibyteCodec_State *state,
		 const unsigned char **inbuf, size_t *inleft,
		 Py_UNICODE **outbuf, size_t *outleft)
{
	/* not written to use encoder, decoder functions because only few
	 * encodings use G2 designations in CJKCodecs */
	if (STATE_G2 == CHARSET_ISO8859_1) {
		ISO8859_1_DECODE(IN3 ^ 0x80, **outbuf)
		else return 3;
	}
	else if (STATE_G2 == CHARSET_ISO8859_7) {
		ISO8859_7_DECODE(IN3 ^ 0x80, **outbuf)
		else return 3;
	}
	else if (STATE_G2 == CHARSET_ASCII) {
		if (IN3 & 0x80) return 3;
		else **outbuf = IN3;
	}
	else
		return MBERR_INTERNAL;

	(*inbuf) += 3;
	*inleft -= 3;
	(*outbuf) += 1;
	*outbuf -= 1;
	return 0;
}

DECODER(iso2022)
{
	const struct iso2022_designation *dsgcache = NULL;
	
	while (inleft > 0) {
		unsigned char c = IN1;
		int err;

		if (STATE_GETFLAG(F_ESCTHROUGHOUT)) {
			/* ESC throughout mode:
			 * for non-iso2022 escape sequences */
			WRITE1(c) /* assume as ISO-8859-1 */
			NEXT(1, 1)
			if (IS_ESCEND(c)) {
				STATE_CLEARFLAG(F_ESCTHROUGHOUT)
			}
			continue;
		}

		switch (c) {
		case ESC:
			RESERVE_INBUF(2)
			if (IS_ISO2022ESC(IN2)) {
				err = iso2022processesc(config, state,
							inbuf, &inleft);
				if (err != 0)
					return err;
			}
			else if (CONFIG_ISSET(USE_G2) && IN2 == 'N') {/* SS2 */
				RESERVE_INBUF(3)
				err = iso2022processg2(config, state,
					inbuf, &inleft, outbuf, &outleft);
				if (err != 0)
					return err;
			}
			else {
				WRITE1(ESC)
				STATE_SETFLAG(F_ESCTHROUGHOUT)
				NEXT(1, 1)
			}
			break;
		case SI:
			if (CONFIG_ISSET(NO_SHIFT))
				goto bypass;
			STATE_CLEARFLAG(F_SHIFTED)
			NEXT_IN(1)
			break;
		case SO:
			if (CONFIG_ISSET(NO_SHIFT))
				goto bypass;
			STATE_SETFLAG(F_SHIFTED)
			NEXT_IN(1)
			break;
		case LF:
			STATE_CLEARFLAG(F_SHIFTED)
			WRITE1(LF)
			NEXT(1, 1)
			break;
		default:
			if (c < 0x20) /* C0 */
				goto bypass;
			else if (c >= 0x80)
				return 1;
			else {
				const struct iso2022_designation *dsg;
				unsigned char charset;
				ucs4_t decoded;

				if (STATE_GETFLAG(F_SHIFTED))
					charset = STATE_G1;
				else
					charset = STATE_G0;

				if (charset == CHARSET_ASCII) {
bypass:					WRITE1(c)
					NEXT(1, 1)
					break;
				}

				if (dsgcache != NULL &&
				    dsgcache->mark == charset)
					dsg = dsgcache;
				else {
					for (dsg = CONFIG_DESIGNATIONS;
					     dsg->mark != charset
#ifdef Py_DEBUG
						&& dsg->mark != '\0'
#endif
					     ;dsg++)
						/* noop */;
					assert(dsg->mark != '\0');
					dsgcache = dsg;
				}

				RESERVE_INBUF(dsg->width)
				decoded = dsg->decoder(*inbuf);
				if (decoded == MAP_UNMAPPABLE)
					return dsg->width;
				WRITE1(decoded)
				NEXT(dsg->width, 1)
			}
			break;
		}
	}
	return 0;
}

/*-*- mapping table holders -*-*/

#define ENCMAP(enc) static const encode_map *enc##_encmap = NULL;
#define DECMAP(enc) static const decode_map *enc##_decmap = NULL;

/* kr */
ENCMAP(cp949)
DECMAP(ksx1001)

/* jp */
ENCMAP(jisxcommon)
DECMAP(jisx0208)
DECMAP(jisx0212)

/* cn */
ENCMAP(gbcommon)
DECMAP(gb2312)

/*-*- mapping access functions -*-*/

int
ksx1001_init(void)
{
	static int ksx1001_initialized = 0;

	if (!ksx1001_initialized && (
			IMPORT_MAP(kr, cp949, &cp949_encmap, NULL) ||
			IMPORT_MAP(kr, ksx1001, NULL, &ksx1001_decmap)))
		return -1;
	ksx1001_initialized = 1;
	return 0;
}

ucs4_t
ksx1001_decoder(const unsigned char *data)
{
	ucs4_t u;
	TRYMAP_DEC(ksx1001, u, data[0], data[1])
		return u;
	else
		return MAP_UNMAPPABLE;
}

DBCHAR
ksx1001_encoder(const ucs4_t *data, int length)
{
	DBCHAR coded;
	assert(length == 1);
	TRYMAP_ENC(cp949, coded, *data)
		if (!(coded & 0x8000))
			return coded;
	return MAP_UNMAPPABLE;
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
