/*
 * codecimpl_cp950.h: the CP950 codec implementation
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
 * $Id: codecimpl_cp950.h,v 1.2 2004/06/27 19:24:13 perky Exp $
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
