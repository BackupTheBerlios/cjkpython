/*
 * codecimple_utf_7.h: the UTF-7 codec (RFC2152) implementation
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
 * $Id: codecimpl_utf_7.h,v 1.1 2004/06/17 18:31:20 perky Exp $
 */

#define SET_DIRECT      1
#define SET_OPTIONAL    2
#define SET_WHITESPACE  3

#define D SET_DIRECT
#define O SET_OPTIONAL
#define W SET_WHITESPACE
static const char utf7_sets[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, W, W, 0, 0, W, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	W, O, O, O, O, O, O, D, D, D, O, 0, D, D, D, 0,
	D, D, D, D, D, D, D, D, D, D, D, O, O, O, O, D,
	O, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,
	D, D, D, D, D, D, D, D, D, D, D, O, 0, O, O, O,
	O, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,
	D, D, D, D, D, D, D, D, D, D, D, O, O, O, 0, 0,
};
#undef W
#undef O
#undef D

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
			RESERVE_INBUF(2)
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
						WRITE3(oc1, '-', c1)
						NEXT_OUT(3)
					} else {
						WRITE2('-', c1)
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
					WRITE1(c1)
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
				RESERVE_INBUF(2)
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

			RESERVE_OUTBUF(1)
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
