/*
 * cjkc_prelude.h: prelude for cjkcodecs
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
 * $Id: cjkc_prelude.h,v 1.1 2004/06/17 18:31:20 perky Exp $
 */

#ifndef _CJKC_PRELUDE_H_
#define _CJKC_PRELUDE_H_

#include "Python.h"
#include "multibytecodec.h"
#include "multibytecodec_compat.h"

#define UNIINV	Py_UNICODE_REPLACEMENT_CHARACTER
#define NOCHAR	0xFFFF
#define MULTIC	0xFFFE
#define DBCINV	0xFFFD

/* short macros to save source size of mapping tables */
#define U UNIINV
#define N NOCHAR
#define M MULTIC
#define D DBCINV

struct dbcs_index {
	const ucs2_t *map;
	unsigned char bottom, top;
};
typedef struct dbcs_index decode_map;

struct widedbcs_index {
	const ucs4_t *map;
	unsigned char bottom, top;
};
typedef struct widedbcs_index widedecode_map;

struct unim_index {
	const DBCHAR *map;
	unsigned char bottom, top;
};
typedef struct unim_index encode_map;

struct dbcs_map {
	const char *charset;
	const struct unim_index *encmap;
	const struct dbcs_index *decmap;
};

struct pair_encodemap {
	ucs4_t uniseq;
	DBCHAR code;
};

#endif
