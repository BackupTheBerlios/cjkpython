/*
 * _codecs_cn.c: Codecs collection for Mainland Chinese encodings
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
 * $Id: _codecs_cn.c,v 1.1 2004/06/17 18:31:20 perky Exp $
 */

#include "cjkc_prelude.h"
#include "maps/map_gb2312.h"
#include "maps/map_gbkext.h"
#include "maps/map_gbcommon.h"
#include "maps/map_gb18030ext.h"
#include "maps/map_gb18030uni.h"
#include "maps/tweak_gbk.h"

#include "cjkc_interlude.h"
#include "codecimpl_gb2312.h"
#include "codecimpl_gbk.h"
#include "codecimpl_gb18030.h"
#include "codecimpl_hz.h"

BEGIN_MAPPING_LIST
  MAPPING_DECONLY(gb2312)
  MAPPING_DECONLY(gbkext)
  MAPPING_ENCONLY(gbcommon)
  MAPPING_ENCDEC(gb18030ext)
END_MAPPING_LIST

BEGIN_CODEC_LIST
  CODEC_STATELESS(gb2312)
  CODEC_STATELESS(gbk)
  CODEC_STATELESS(gb18030)
  CODEC_STATEFUL(hz)
END_CODEC_LIST

#include "cjkc_postlude.h"

I_AM_A_MODULE_FOR(cn)