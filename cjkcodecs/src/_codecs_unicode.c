/*
 * _codecs_unicode.c: Codecs collection for Unicode encodings
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
 * $Id: _codecs_unicode.c,v 1.6 2004/08/07 08:12:31 perky Exp $
 */

#include "cjkcodecs.h"

/*
 * UTF-7 codec
 */

#define SET_DIRECT      1
#define SET_OPTIONAL    2
#define SET_WHITESPACE  3

#define _D SET_DIRECT
#define _O SET_OPTIONAL
#define _W SET_WHITESPACE
static const char utf7_sets[128] = {
	 0,  0,  0,  0,  0,  0,  0,  0,  0, _W, _W,  0,  0, _W,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	_W, _O, _O, _O, _O, _O, _O, _D, _D, _D, _O,  0, _D, _D, _D,  0,
	_D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _O, _O, _O, _O, _D,
	_O, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D,
	_D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _O,  0, _O, _O, _O,
	_O, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D,
	_D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _D, _O, _O, _O,  0,  0,
};
#undef _W
#undef _O
#undef _D

#define B64(n)  ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" \
		"0123456789+/"[(n) & 0x3f])
#define B64CHAR(c)	(((c) >= 'A' && (c) <= 'Z') || \
			((c) >= 'a' && (c) <= 'z') || \
			((c) >= '0' && (c) <= '9') || \
			(c) == '+' || (c) == '/')
#define UB64(c) ((c) == '+' ? 62 : (c) == '/' ? 63 : (c) >= 'a' ? \
		(c) - 71 : (c) >= 'A' ? (c) - 65 : (c) + 4)

#define UTF7_DENCODABLE_COMPATIBLE(c)   (utf7_sets[c] != 0)
#define UTF7_DENCODABLE_STRICT(c)       (utf7_sets[c] == SET_DIRECT || \
					 utf7_sets[c] == SET_WHITESPACE)

#define ESTATE_INITIALIZE(state) \
	ESTATE_SETSTAGE(state, 0) \
	ESTATE_CLEARSHIFTED(state)

#define ESTATE_SETPENDING(state, v)	(state)->c[0] = (v);
#define ESTATE_GETPENDING(state)	(state)->c[0]

#define ESTATE_SETSHIFTED(state)	(state)->c[2] = 1;
#define ESTATE_ISSHIFTED(state)		((state)->c[2])
#define ESTATE_CLEARSHIFTED(state)	(state)->c[2] = 0;

#define ESTATE_SETSTAGE(state, v)	(state)->c[3] = (v);
#define ESTATE_GETSTAGE(state)		((state)->c[3])

ENCODER_INIT(utf_7)
{
	ESTATE_INITIALIZE(state)
	return 0;
}

ENCODER_RESET(utf_7)
{
	if (ESTATE_ISSHIFTED(state)) {
		if (ESTATE_GETSTAGE(state) != 0) {
			unsigned char oc;

			oc = B64(ESTATE_GETPENDING(state));
			WRITE2(oc, '-')
			NEXT_OUT(2)
		}
		else {
			WRITE1('-')
			NEXT_OUT(1)
		}
		ESTATE_CLEARSHIFTED(state)
	}
	return 0;
}

ENCODER(utf_7)
{
	while (inleft > 0) {
		Py_UNICODE c1 = IN1, c2 = 0;
		size_t insize = 1;

#if Py_UNICODE_SIZE == 2
		if (c1 >> 10 == 0xd800 >> 10) { /* high surrogate */
			REQUIRE_INBUF(2)
			if (IN2 >> 10 != 0xdc00 >> 10) /* low surrogate */
				return 2; /* invalid surrogate pair */
			c2 = IN2;
			insize = 2;
		}
#else
		if (c1 > 0x10ffff) /* UTF-16 unencodable */
			return 1;
		else if (c1 > 0xffff) {
			c2 = 0xdc00 | ((c1 - 0x10000) & 0x3ff);
			c1 = 0xd800 | ((c1 - 0x10000) >> 10);
		}
#endif

		for (;;) {
			unsigned char oc1, oc2, oc3;

			if (ESTATE_ISSHIFTED(state)) {
				if (c1 < 128 && UTF7_DENCODABLE_STRICT(c1)) {
					if (ESTATE_GETSTAGE(state) != 0) {
						oc1 = B64(ESTATE_GETPENDING(
								state));
						WRITE3(oc1, '-',
						       (unsigned char)c1)
						NEXT_OUT(3)
					} else {
						WRITE2('-',
						       (unsigned char)c1)
						NEXT_OUT(2)
					}
					ESTATE_CLEARSHIFTED(state)
				} else {
					switch (ESTATE_GETSTAGE(state)) {
					case 0:
						oc1 = c1 >> 10;
						oc2 = (c1 >> 4) & 0x3f;
						WRITE2(B64(oc1), B64(oc2))
						ESTATE_SETPENDING(state,
							(c1 & 0x0f) << 2)
						ESTATE_SETSTAGE(state, 2)
						NEXT_OUT(2)
						break;
					case 1:
						oc1 = ESTATE_GETPENDING(state)
							| (c1 >> 12);
						oc2 = (c1 >> 6) & 0x3f;
						oc3 = c1 & 0x3f;
						WRITE3(B64(oc1), B64(oc2),
							B64(oc3))
						ESTATE_SETSTAGE(state, 0)
						NEXT_OUT(3)
						break;
					case 2:
						oc1 = ESTATE_GETPENDING(state)
							| (c1 >> 14);
						oc2 = (c1 >> 8) & 0x3f;
						oc3 = (c1 >> 2) & 0x3f;
						WRITE3(B64(oc1), B64(oc2),
							B64(oc3))
						ESTATE_SETPENDING(state,
							(c1 & 0x03) << 4)
						ESTATE_SETSTAGE(state, 1)
						NEXT_OUT(3)
						break;
					default:
						return MBERR_INTERNAL;
					}
				}
			}
			else {
				if (c1 < 128 && UTF7_DENCODABLE_STRICT(c1)) {
					WRITE1((unsigned char)c1)
					NEXT_OUT(1)
				}
				else if (c1 == '+') {
					WRITE2('+', '-')
					NEXT_OUT(2)
				}
				else {
					oc1 = c1 >> 10;
					oc2 = (c1 >> 4) & 0x3f;
					WRITE3('+', B64(oc1), B64(oc2))
					ESTATE_SETPENDING(state,
							  (c1 & 0x0f) << 2)
					ESTATE_SETSTAGE(state, 2)
					ESTATE_SETSHIFTED(state)
					NEXT_OUT(3)
				}
			}

			if (c2 != 0) {
				c1 = c2;
				c2 = 0;
			}
			else
				break;
		}

		NEXT_IN(insize)
	}

	return 0;
}

#define DSTATE_INITIALIZE(state)	\
	DSTATE_SETBSTAGE(state, 0)	\
	DSTATE_CLEARSHIFTED(state)	\
	DSTATE_SETULENGTH(state, 0)	\
	DSTATE_SETUPENDING1(state, 0)	\
	DSTATE_SETUPENDING2(state, 0)

/* XXX: Type-mixed usage of a state union may be not so portable.
 * If you see any problem with this on your platfom. Please let
 * me know. */

#define DSTATE_SETSHIFTED(state)	(state)->c[0] = 1;
#define DSTATE_ISSHIFTED(state)		((state)->c[0])
#define DSTATE_CLEARSHIFTED(state)	(state)->c[0] = 0;

#define DSTATE_SETBSTAGE(state, v)	(state)->c[1] = (v);
#define DSTATE_GETBSTAGE(state)		((state)->c[1])

#define DSTATE_SETBPENDING(state, v)	(state)->c[2] = (v);
#define DSTATE_GETBPENDING(state)	((state)->c[2])

#define DSTATE_SETULENGTH(state, v)	(state)->c[3] = (v);
#define DSTATE_GETULENGTH(state)	((state)->c[3])

#define DSTATE_SETUPENDING1(state, v)	(state)->u2[2] = (v);
#define DSTATE_GETUPENDING1(state)	(state)->u2[2]

#define DSTATE_SETUPENDING2(state, v)	(state)->u2[3] = (v);
#define DSTATE_GETUPENDING2(state)	(state)->u2[3]

#define DSTATE_UAPPEND(state, v)				\
	(state)->u2[(state)->c[3] > 1 ? 3 : 2] |=		\
		((state)->c[3] & 1) ? (v) : ((ucs2_t)(v)) << 8; \
	(state)->c[3]++;

DECODER_INIT(utf_7)
{
    DSTATE_INITIALIZE(state)
    return 0;
}

static int
utf_7_flush(MultibyteCodec_State *state,
	    Py_UNICODE **outbuf, size_t *outleft)
{
	switch (DSTATE_GETULENGTH(state)) {
	case 2: {
		ucs2_t   uc;

		uc = DSTATE_GETUPENDING1(state);
#if Py_UNICODE_SIZE == 4
		if (uc >> 10 == 0xd800 >> 10)
			return MBERR_TOOFEW;
#endif
		OUT1(uc)
		(*outbuf)++;
		(*outleft)--;
		DSTATE_SETULENGTH(state, 0)
		DSTATE_SETUPENDING1(state, 0)
		break;
	}
#if Py_UNICODE_SIZE == 4
	case 4:
		if (DSTATE_GETUPENDING2(state) >> 10 != 0xdc00 >> 10)
			return 1;
		OUT1(0x10000 + (((ucs4_t)DSTATE_GETUPENDING1(state) - 0xd800)
				<< 10) + (DSTATE_GETUPENDING2(state) - 0xdc00))
		(*outbuf)++;
		(*outleft)--;
		DSTATE_SETULENGTH(state, 0)
		DSTATE_SETUPENDING1(state, 0)
		DSTATE_SETUPENDING2(state, 0)
		break;
#endif
	case 0: /* FALLTHROUGH */
	case 1: /* FALLTHROUGH */
	case 3:
		return MBERR_TOOFEW;
	default:
		return MBERR_INTERNAL;
	}

	return 0;
}

DECODER_RESET(utf_7)
{
	DSTATE_INITIALIZE(state)
	return 0;
}

DECODER(utf_7)
{
	while (inleft > 0) {
		unsigned char c = IN1;
		int r;

		if (!DSTATE_ISSHIFTED(state)) {
			if (c == '+') {
				REQUIRE_INBUF(2)
				if (inleft >= 2 && IN2 == '-') {
					WRITE1('+')
					NEXT(2, 1)
				}
				else {
					DSTATE_SETSHIFTED(state)
					NEXT_IN(1)
				}
			}
			else if (c < 128 && UTF7_DENCODABLE_COMPATIBLE(c)) {
				WRITE1(c)
				NEXT(1, 1)
			}
			else
				return 1;
		}
		else if (B64CHAR(c)) {
			unsigned char tb;

			REQUIRE_OUTBUF(1)
			c = UB64(c);
			assert(DSTATE_GETULENGTH(state) < 4);

			switch (DSTATE_GETBSTAGE(state)) {
			case 0:
				DSTATE_SETBPENDING(state, c << 2)
				DSTATE_SETBSTAGE(state, 1)
				break;
			case 1:
				tb = DSTATE_GETBPENDING(state) | (c >> 4);
				DSTATE_SETBPENDING(state, c << 4)
				DSTATE_SETBSTAGE(state, 2)
				DSTATE_UAPPEND(state, tb)
				break;
			case 2:
				tb = DSTATE_GETBPENDING(state) | (c >> 2);
				DSTATE_SETBPENDING(state, c << 6)
				DSTATE_SETBSTAGE(state, 3)
				DSTATE_UAPPEND(state, tb)
				break;
			case 3:
				tb = DSTATE_GETBPENDING(state) | c;
				DSTATE_SETBSTAGE(state, 0)
				DSTATE_UAPPEND(state, tb)
				break;
			}

			r = utf_7_flush(state, outbuf, &outleft);
			if (r != 0 && r != MBERR_TOOFEW)
				return r;
			NEXT_IN(1)
		}
		else if (c == '-' || UTF7_DENCODABLE_COMPATIBLE(c)) {
			if (DSTATE_GETBSTAGE(state) != 0) {
				DSTATE_UAPPEND(state, DSTATE_GETBSTAGE(state))
				DSTATE_SETBSTAGE(state, 0)
			}
			r = utf_7_flush(state, outbuf, &outleft);
			if (r != 0 && r != MBERR_TOOFEW)
				return r;
			DSTATE_CLEARSHIFTED(state)

			if (c != '-') {
				WRITE1(c)
				NEXT_OUT(1)
			}
			NEXT_IN(1)
		}
		else
			return 1;
	}

	return 0;
}


/*
 * UTF-8 codec
 */

ENCODER(utf_8)
{
	while (inleft > 0) {
		ucs4_t c = **inbuf;
		size_t outsize, insize = 1;

		if (c < 0x80) outsize = 1;
		else if (c < 0x800) outsize = 2;
		else {
#if Py_UNICODE_SIZE == 2
			if (c >> 10 == 0xd800 >> 10) { /* high surrogate */
				if (inleft < 2) {
					if (!(flags & MBENC_FLUSH))
						return MBERR_TOOFEW;
				}
				else if ((*inbuf)[1] >> 10 == 0xdc00 >> 10) {
					/* low surrogate */
					c = 0x10000 + ((c - 0xd800) << 10) +
					    ((ucs4_t)((*inbuf)[1]) - 0xdc00);
					insize = 2;
				}
			}
#endif
			if (c < 0x10000) outsize = 3;
			else if (c < 0x200000) outsize = 4;
			else if (c < 0x4000000) outsize = 5;
			else outsize = 6;
		}

		REQUIRE_OUTBUF(outsize)

		switch (outsize) {
		case 6:
			(*outbuf)[5] = 0x80 | (c & 0x3f);
			c = c >> 6;
			c |= 0x4000000;
			/* FALLTHROUGH */
		case 5:
			(*outbuf)[4] = 0x80 | (c & 0x3f);
			c = c >> 6;
			c |= 0x200000;
			/* FALLTHROUGH */
		case 4:
			(*outbuf)[3] = 0x80 | (c & 0x3f);
			c = c >> 6;
			c |= 0x10000;
			/* FALLTHROUGH */
		case 3:
			(*outbuf)[2] = 0x80 | (c & 0x3f);
			c = c >> 6;
			c |= 0x800;
			/* FALLTHROUGH */
		case 2:
			(*outbuf)[1] = 0x80 | (c & 0x3f);
			c = c >> 6;
			c |= 0xc0;
			/* FALLTHROUGH */
		case 1:
			(*outbuf)[0] = c;
		}

		NEXT(insize, outsize)
	}

	return 0;
}

DECODER(utf_8)
{
	while (inleft > 0) {
		unsigned char c = **inbuf;

		REQUIRE_OUTBUF(1)

		if (c < 0x80) {
			(*outbuf)[0] = (unsigned char)c;
			NEXT(1, 1)
		}
		else if (c < 0xc2) {
			return 1;
		}
		else if (c < 0xe0) {
			unsigned char c2;

			REQUIRE_INBUF(2)
				c2 = (*inbuf)[1];
			if (!((c2 ^ 0x80) < 0x40))
				return 2;
			**outbuf = ((Py_UNICODE)(c & 0x1f) << 6) |
				    (Py_UNICODE)(c2 ^ 0x80);
			NEXT(2, 1)
		}
		else if (c < 0xf0) {
			unsigned char c2, c3;

			REQUIRE_INBUF(3)
			c2 = (*inbuf)[1]; c3 = (*inbuf)[2];
			if (!((c2 ^ 0x80) < 0x40 &&
			    (c3 ^ 0x80) < 0x40 && (c >= 0xe1 || c2 >= 0xa0)))
				return 3;
			**outbuf = ((Py_UNICODE)(c & 0x0f) << 12)
				| ((Py_UNICODE)(c2 ^ 0x80) << 6)
				| (Py_UNICODE)(c3 ^ 0x80);
			NEXT(3, 1)
		}
		else if (c < 0xf8) {
			unsigned char c2, c3, c4;
			ucs4_t code;

			REQUIRE_INBUF(4)
			c2 = (*inbuf)[1]; c3 = (*inbuf)[2];
			c4 = (*inbuf)[3];
			if (!((c2 ^ 0x80) < 0x40 &&
			    (c3 ^ 0x80) < 0x40 && (c4 ^ 0x80) < 0x40 &&
			    (c >= 0xf1 || c2 >= 0x90)))
				return 4;
			code = ((ucs4_t)(c & 0x07) << 18)
				| ((ucs4_t)(c2 ^ 0x80) << 12)
				| ((ucs4_t)(c3 ^ 0x80) << 6)
				| (ucs4_t)(c4 ^ 0x80);
			WRITEUCS4(code)
			NEXT_IN(4)
		}
		else if (c < 0xfc) {
			unsigned char c2, c3, c4, c5;
			ucs4_t code;

			REQUIRE_INBUF(5)
			c2 = (*inbuf)[1]; c3 = (*inbuf)[2];
			c4 = (*inbuf)[3]; c5 = (*inbuf)[4];
			if (!((c2 ^ 0x80) < 0x40 &&
			    (c3 ^ 0x80) < 0x40 && (c4 ^ 0x80) < 0x40 &&
			    (c5 ^ 0x80) < 0x40 && (c >= 0xf9 || c2 >= 0x88)))
				return 5;
			code = ((ucs4_t)(c & 0x03) << 24)
				| ((ucs4_t)(c2 ^ 0x80) << 18)
				| ((ucs4_t)(c3 ^ 0x80) << 12)
				| ((ucs4_t)(c4 ^ 0x80) << 6)
				| (ucs4_t)(c5 ^ 0x80);
			WRITEUCS4(code)
			NEXT_IN(5)
		}
		else if (c < 0xff) {
			unsigned char c2, c3, c4, c5, c6;
			ucs4_t code;

			REQUIRE_INBUF(6)
			c2 = (*inbuf)[1]; c3 = (*inbuf)[2];
			c4 = (*inbuf)[3]; c5 = (*inbuf)[4];
			c6 = (*inbuf)[5];
			if (!((c2 ^ 0x80) < 0x40 &&
			    (c3 ^ 0x80) < 0x40 && (c4 ^ 0x80) < 0x40 &&
			    (c5 ^ 0x80) < 0x40 && (c6 ^ 0x80) < 0x40 &&
			    (c >= 0xfd || c2 >= 0x84)))
				return 6;
			code = ((ucs4_t)(c & 0x01) << 30)
				| ((ucs4_t)(c2 ^ 0x80) << 24)
				| ((ucs4_t)(c3 ^ 0x80) << 18)
				| ((ucs4_t)(c4 ^ 0x80) << 12)
				| ((ucs4_t)(c5 ^ 0x80) << 6)
				| (ucs4_t)(c6 ^ 0x80);
			WRITEUCS4(code)
			NEXT_IN(6)
		}
		else
			return 1;
	}

	return 0;
}


#define UTF16_ENCODE_PREAMBLE					\
	while (inleft > 0) {					\
		Py_UNICODE c = IN1;				\
		ucs2_t uc;					\
		unsigned char *ucdata = (unsigned char *)&uc;

#define UTF16_ENCODE_BODY		\
		_UTF16_ENCODE_UCS4	\
		REQUIRE_OUTBUF(2)	\
		uc = c;			\
		OUT1(ucdata[BYTE0])	\
		OUT2(ucdata[BYTE1])	\
		NEXT(1, 2)		\
	}				\
	return 0;

#if Py_UNICODE_SIZE == 4
# define _UTF16_ENCODE_UCS4					\
	if (c > 0xffff) {					\
		if (c > 0x10ffff)				\
			return 1;				\
		REQUIRE_OUTBUF(4)				\
		uc = 0xd800 + (((c) - 0x10000) >> 10);		\
		OUT1(ucdata[BYTE0])				\
		OUT2(ucdata[BYTE1])				\
		uc = 0xdc00 + (((c) - 0x10000) & 0x3ff);	\
		OUT3(ucdata[BYTE0])				\
		OUT4(ucdata[BYTE1])				\
		NEXT(1, 4)					\
		continue;					\
	}
#else
# define _UTF16_ENCODE_UCS4
#endif


#define UTF16_DECODE_PREAMBLE					\
	while (inleft > 0) {					\
		ucs2_t uc;					\
		unsigned char *c = (unsigned char *)&uc;	\
		REQUIRE_INBUF(2)				\

#define UTF16_DECODE_BODY					\
		REQUIRE_OUTBUF(1)				\
		UTF16_DECGET(c, IN1, IN2)			\
		UTF16_DECODE_UCS4				\
		OUT1(uc)					\
		NEXT(2, 1)					\
	}							\
	return 0;

#if Py_UNICODE_SIZE == 4
# define UTF16_DECODE_UCS4					\
	if (uc >> 10 == 0xd800 >> 10) {				\
		ucs2_t uc2;					\
		unsigned char *c2 = (unsigned char *)&uc2;	\
								\
		REQUIRE_INBUF(4)				\
		UTF16_DECGET(c2, IN3, IN4)			\
		if (uc2 >> 10 != 0xdc00 >> 10)			\
			return 4;				\
		OUT1(0x10000 + ((ucs4_t)(uc - 0xd800) << 10) +	\
		     (uc2 - 0xdc00))				\
		NEXT(4, 1)					\
		continue;					\
	}
#else
# define UTF16_DECODE_UCS4
#endif

/*
 * UTF-16 codec
 */

#ifdef WORDS_BIGENDIAN
# define UTF16_NATIVE 1
#else
# define UTF16_NATIVE -1
#endif

#define BYTE0 0
#define BYTE1 1

#define UTF16_DECGET(assi, b1, b2)			\
	if (state->i == UTF16_NATIVE)			\
		(assi)[0] = (b1), (assi)[1] = (b2);	\
	else						\
		(assi)[1] = (b1), (assi)[0] = (b2);	\

ENCODER_INIT(utf_16)
{
	state->i = 1; /* indicates "the beginning of stream" */
	return 0;
}

ENCODER_RESET(utf_16)
{
	return 0;
}

ENCODER(utf_16)
{
	UTF16_ENCODE_PREAMBLE
	if (state->i) { /* BOM */
		state->i = 0;
		uc = 0xfeff;
		WRITE2(ucdata[0], ucdata[1])
		NEXT_OUT(2)
	}
	UTF16_ENCODE_BODY
}

DECODER_INIT(utf_16)
{
	state->i = 0; /* -1: little 0: (the beginning of stream) 1: big */
	return 0;
}

DECODER_RESET(utf_16)
{
	return 0;
}

DECODER(utf_16)
{
	UTF16_DECODE_PREAMBLE
	if (state->i == 0) {
		if (IN1 == 0xff && IN2 == 0xfe) {
			state->i = -1;
			NEXT_IN(2)
			continue;
		}
		else if (IN1 == 0xfe && IN2 == 0xff) {
			state->i = 1;
			NEXT_IN(2)
			continue;
		}
		else {	/* set as native byte order and don't
			 * interpret any more BOMs
			 * (RFC2781 Page 4) */
			state->i = UTF16_NATIVE;
		}
	}
	UTF16_DECODE_BODY
}

#undef BYTE0
#undef BYTE1
#undef UTF16_DECGET


/*
 * UTF-16BE codec
 */

#ifdef WORDS_BIGENDIAN
# define BYTE0  0
# define BYTE1  1
#else
# define BYTE0  1
# define BYTE1  0
#endif

#define UTF16_DECGET(assi, b1, b2)	\
	(assi)[BYTE0] = (b1);		\
	(assi)[BYTE1] = (b2);

ENCODER(utf_16_be)
{
	UTF16_ENCODE_PREAMBLE
	UTF16_ENCODE_BODY
}

DECODER(utf_16_be)
{
	UTF16_DECODE_PREAMBLE
	UTF16_DECODE_BODY
}

#undef BYTE0
#undef BYTE1
#undef UTF16_DECGET


/*
 * UTF-16LE codec
 */

#ifdef WORDS_BIGENDIAN
# define BYTE0  1
# define BYTE1  0
#else
# define BYTE0  0
# define BYTE1  1
#endif

#define UTF16_DECGET(assi, b1, b2)	\
	(assi)[BYTE0] = (b1);		\
	(assi)[BYTE1] = (b2);

ENCODER(utf_16_le)
{
	UTF16_ENCODE_PREAMBLE
	UTF16_ENCODE_BODY
}

DECODER(utf_16_le)
{
	UTF16_DECODE_PREAMBLE
	UTF16_DECODE_BODY
}

#undef BYTE0
#undef BYTE1
#undef UTF16_DECGET

BEGIN_MAPPINGS_LIST
END_MAPPINGS_LIST

BEGIN_CODECS_LIST
  CODEC_STATEFUL(utf_7)
  CODEC_STATELESS(utf_8)
  CODEC_STATEFUL(utf_16) /* not so stateful, but for implementation */
  CODEC_STATELESS(utf_16_be)
  CODEC_STATELESS(utf_16_le)
END_CODECS_LIST

I_AM_A_MODULE_FOR(unicode)
