/*
 * _codecs_jp.c: Codecs collection for Japanese encodings
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
 * $Id: _codecs_jp.c,v 1.1 2004/06/17 18:31:20 perky Exp $
 */

#define USING_BINARY_PAIR_SEARCH
#define EMPBASE 0x20000

#include "cjkc_prelude.h"
#include "maps/map_jisx0208.h"
#include "maps/map_jisx0212.h"
#include "maps/map_jisx0213.h"
#include "maps/map_jisxcommon.h"
#include "maps/map_cp932ext.h"
#include "maps/map_jisx0213_pairs.h"
#include "maps/alg_jisx0201.h"

#include "cjkc_interlude.h"
#include "codecimpl_cp932.h"
#include "codecimpl_euc_jisx0213.h"
#include "codecimpl_euc_jp.h"
#include "codecimpl_shift_jis.h"
#include "codecimpl_shift_jisx0213.h"

BEGIN_MAPPING_LIST
  MAPPING_DECONLY(jisx0208)
  MAPPING_DECONLY(jisx0212)
  MAPPING_ENCONLY(jisxcommon)
  MAPPING_DECONLY(jisx0213_1_bmp)
  MAPPING_DECONLY(jisx0213_2_bmp)
  MAPPING_ENCONLY(jisx0213_bmp)
  MAPPING_DECONLY(jisx0213_1_emp)
  MAPPING_DECONLY(jisx0213_2_emp)
  MAPPING_ENCONLY(jisx0213_emp)
  MAPPING_ENCDEC(cp932ext)
END_MAPPING_LIST

BEGIN_CODEC_LIST
  CODEC_STATELESS(shift_jis)
  CODEC_STATELESS(cp932)
  CODEC_STATELESS(euc_jp)
  CODEC_STATELESS(shift_jisx0213)
  CODEC_STATELESS(euc_jisx0213)
END_CODEC_LIST

#include "cjkc_postlude.h"

I_AM_A_MODULE_FOR(jp)
