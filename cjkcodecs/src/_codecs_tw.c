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
 * $Id: _codecs_tw.c,v 1.4 2004/06/27 21:03:54 perky Exp $
 */

#include "cjkc_prelude.h"
#include "maps/map_big5.h"
#include "maps/map_cp950ext.h"
#ifndef NO_EXTRA_ENCODINGS
# include "maps/map_cns11643.h"
#endif

#include "cjkc_interlude.h"
#include "impl_big5.h"
#include "impl_cp950.h"
#ifndef NO_EXTRA_ENCODINGS
# include "impl_euc_tw.h"
#endif

BEGIN_MAPPING_LIST
  MAPPING_ENCDEC(big5)
  MAPPING_ENCDEC(cp950ext)
#ifndef NO_EXTRA_ENCODINGS
  /* these are non-regular tables and not used by external codecs yet.
  MAPPING_ENCONLY(cns11643_bmp)
  MAPPING_ENCONLY(cns11643_nonbmp)*/
  MAPPING_DECONLY(cns11643_1)
  MAPPING_DECONLY(cns11643_2)
  MAPPING_DECONLY(cns11643_3)
  MAPPING_DECONLY(cns11643_4)
  MAPPING_DECONLY(cns11643_5)
  MAPPING_DECONLY(cns11643_6)
  MAPPING_DECONLY(cns11643_7)
#endif
END_MAPPING_LIST

BEGIN_CODEC_LIST
  CODEC_STATELESS(big5)
  CODEC_STATELESS(cp950)
#ifndef NO_EXTRA_ENCODINGS
  CODEC_STATELESS(euc_tw)
#endif
END_CODEC_LIST

#include "cjkc_postlude.h"

I_AM_A_MODULE_FOR(tw)
