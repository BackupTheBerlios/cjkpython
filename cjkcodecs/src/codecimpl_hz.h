/*
 * codecimpl_hz.h: the HZ codec (RFC1843) implementation
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
 * $Id: codecimpl_hz.h,v 1.1 2004/06/17 18:31:20 perky Exp $
 */

ENCODER_INIT(hz)
{
	state->i = 0;
	return 0;
}

ENCODER_RESET(hz)
{
	if (state->i != 0) {
		WRITE2('~', '}')
		state->i = 0;
		NEXT_OUT(2)
	}
	return 0;
}

ENCODER(hz)
{
	while (inleft > 0) {
		Py_UNICODE c = IN1;
		DBCHAR code;

		if (c < 0x80) {
			if (state->i == 0) {
				WRITE1((unsigned char)c)
				NEXT(1, 1)
			}
			else {
				WRITE3('~', '}', (unsigned char)c)
				NEXT(1, 3)
				state->i = 0;
			}
			continue;
		}

		UCS4INVALID(c)

		TRYMAP_ENC(gbcommon, code, c);
		else return 1;

		if (code & 0x8000) /* MSB set: GBK */
			return 1;

		if (state->i == 0) {
			WRITE4('~', '{', code >> 8, code & 0xff)
			NEXT(1, 4)
			state->i = 1;
		}
		else {
			WRITE2(code >> 8, code & 0xff)
			NEXT(1, 2)
		}
	}

	return 0;
}

DECODER_INIT(hz)
{
	state->i = 0;
	return 0;
}

DECODER_RESET(hz)
{
	state->i = 0;
	return 0;
}

DECODER(hz)
{
	while (inleft > 0) {
		unsigned char c = IN1;

		if (c == '~') {
			unsigned char c2 = IN2;

			RESERVE_INBUF(2)
			if (c2 == '~') {
				WRITE1('~')
				NEXT(2, 1)
				continue;
			}
			else if (c2 == '{' && state->i == 0)
				state->i = 1; /* set GB */
			else if (c2 == '}' && state->i == 1)
				state->i = 0; /* set ASCII */
			else if (c2 == '\n')
				; /* line-continuation */
			else
				return 2;
			NEXT(2, 0);
			continue;
		}

		if (c & 0x80)
			return 1;

		if (state->i == 0) { /* ASCII mode */
			WRITE1(c)
			NEXT(1, 1)
		}
		else { /* GB mode */
			RESERVE_INBUF(2)
			RESERVE_OUTBUF(1)
			TRYMAP_DEC(gb2312, **outbuf, c, IN2) {
				NEXT(2, 1)
			}
			else
				return 2;
		}
	}

	return 0;
}
