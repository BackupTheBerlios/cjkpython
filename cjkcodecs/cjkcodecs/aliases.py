#
# aliases.py: Encoding Name Aliases for CJK Codecs
#
# Copyright (C) 2003-2004 Hye-Shik Chang <perky@FreeBSD.org>.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# $Id: aliases.py,v 1.11 2004/06/28 18:16:03 perky Exp $
#

from encodings.aliases import aliases

aliases.update({
    # big5 codec
    'big5'               : 'cjkcodecs.big5',
    'big5_tw'            : 'cjkcodecs.big5',
    'csbig5'             : 'cjkcodecs.big5',

    # big5hkscs codec
    'big5_hkscs'         : 'cjkcodecs.big5hkscs',
    'big5hkscs'          : 'cjkcodecs.big5hkscs',
    'hkscs'              : 'cjkcodecs.big5hkscs',

    # cp932 codec
    '932'                : 'cjkcodecs.cp932',
    'cp932'              : 'cjkcodecs.cp932',
    'ms932'              : 'cjkcodecs.cp932',
    'mskanji'            : 'cjkcodecs.cp932',
    'ms_kanji'           : 'cjkcodecs.cp932',
    'windows_31j'        : 'cjkcodecs.cp932',

    # cp949 codec
    '949'                : 'cjkcodecs.cp949',
    'cp949'              : 'cjkcodecs.cp949',
    'ms949'              : 'cjkcodecs.cp949',
    'uhc'                : 'cjkcodecs.cp949',

    # cp950 codec
    '950'                : 'cjkcodecs.cp950',
    'cp950'              : 'cjkcodecs.cp950',
    'ms950'              : 'cjkcodecs.cp950',

    # euc_jisx0213 codec
    'jisx0213'           : 'cjkcodecs.euc_jisx0213',
    'eucjisx0213'        : 'cjkcodecs.euc_jisx0213',
    'euc_jisx0213'       : 'cjkcodecs.euc_jisx0213',

    # euc_jisx0213_strict codec
    'euc_jisx0213_strict': 'cjkcodecs.euc_jisx0213_strict',

    # euc_jp codec
    'eucjp'              : 'cjkcodecs.euc_jp',
    'euc_jp'             : 'cjkcodecs.euc_jp',
    'ujis'               : 'cjkcodecs.euc_jp',
    'u_jis'              : 'cjkcodecs.euc_jp',

    # euc_jp_strict codec
    'euc_jp_strict'      : 'cjkcodecs.euc_jp_strict',

    # euc_kr codec
    'euckr'              : 'cjkcodecs.euc_kr',
    'euc_kr'             : 'cjkcodecs.euc_kr',
    'korean'             : 'cjkcodecs.euc_kr',
    'ksc5601'            : 'cjkcodecs.euc_kr',
    'ks_c_5601'          : 'cjkcodecs.euc_kr',
    'ks_c_5601_1987'     : 'cjkcodecs.euc_kr',
    'ksx1001'            : 'cjkcodecs.euc_kr',
    'ks_x_1001'          : 'cjkcodecs.euc_kr',

    # euc_tw codec
    'cns11643'           : 'cjkcodecs.euc_tw',
    'cseuctw'            : 'cjkcodecs.euc_tw',
    'euctw'              : 'cjkcodecs.euc_tw',
    'euc_tw'             : 'cjkcodecs.euc_tw',

    # gb18030 codec
    'gb18030'            : 'cjkcodecs.gb18030',
    'gb18030_2000'       : 'cjkcodecs.gb18030',

    # gb2312 codec
    'chinese'            : 'cjkcodecs.gb2312',
    'csISO58gb231280'    : 'cjkcodecs.gb2312',
    'euc_cn'             : 'cjkcodecs.gb2312',
    'euccn'              : 'cjkcodecs.gb2312',
    'eucgb2312_cn'       : 'cjkcodecs.gb2312',
    'gb2312'             : 'cjkcodecs.gb2312',
    'gb2312_1980'        : 'cjkcodecs.gb2312',
    'gb2312_80'          : 'cjkcodecs.gb2312',
    'iso_ir_58'          : 'cjkcodecs.gb2312',

    # gbk codec
    '936'                : 'cjkcodecs.gbk',
    'cp936'              : 'cjkcodecs.gbk',
    'gbk'                : 'cjkcodecs.gbk',
    'ms936'              : 'cjkcodecs.gbk',

    # hz codec
    'hz'                 : 'cjkcodecs.hz',
    'hzgb'               : 'cjkcodecs.hz',
    'hz_gb'              : 'cjkcodecs.hz',
    'hz_gb_2312'         : 'cjkcodecs.hz',

    # iso2022_jp codec
    'iso_2022_jp'        : 'cjkcodecs.iso2022_jp',
    'iso2022_jp'         : 'cjkcodecs.iso2022_jp',
    'iso2022jp'          : 'cjkcodecs.iso2022_jp',

    # iso2022_jp_1 codec
    'iso_2022_jp_1'      : 'cjkcodecs.iso2022_jp_1',
    'iso2022_jp_1'       : 'cjkcodecs.iso2022_jp_1',
    'iso2022jp_1'        : 'cjkcodecs.iso2022_jp_1',

    # iso2022_jp_2 codec
    'iso_2022_jp_2'      : 'cjkcodecs.iso2022_jp_2',
    'iso2022_jp_2'       : 'cjkcodecs.iso2022_jp_2',
    'iso2022jp_2'        : 'cjkcodecs.iso2022_jp_2',

    # iso2022_jp_3 codec
    'iso_2022_jp_3'      : 'cjkcodecs.iso2022_jp_3',
    'iso2022_jp_3'       : 'cjkcodecs.iso2022_jp_3',
    'iso2022jp_3'        : 'cjkcodecs.iso2022_jp_3',

    # iso2022_jp_ext codec
    'iso_2022_jp_ext'    : 'cjkcodecs.iso2022_jp_ext',
    'iso2022_jp_ext'     : 'cjkcodecs.iso2022_jp_ext',
    'iso2022jp_ext'      : 'cjkcodecs.iso2022_jp_ext',

    # iso2022_kr codec
    'iso_2022_kr'        : 'cjkcodecs.iso2022_kr',
    'iso2022_kr'         : 'cjkcodecs.iso2022_kr',
    'iso2022kr'          : 'cjkcodecs.iso2022_kr',

    # johab codec
    'johab'              : 'cjkcodecs.johab',
    'cp1361'             : 'cjkcodecs.johab',
    'ms1361'             : 'cjkcodecs.johab',

    # shift_jis codec
    'csshiftjis'         : 'cjkcodecs.shift_jis',
    'shiftjis'           : 'cjkcodecs.shift_jis',
    'shift_jis'          : 'cjkcodecs.shift_jis',
    'sjis'               : 'cjkcodecs.shift_jis',
    's_jis'              : 'cjkcodecs.shift_jis',

    # shift_jis_strict codec
    'shift_jis_strict'   : 'cjkcodecs.shift_jis_strict',

    # shift_jisx0213 codec
    'shiftjisx0213'      : 'cjkcodecs.shift_jisx0213',
    'shift_jisx0213'     : 'cjkcodecs.shift_jisx0213',
    'sjisx0213'          : 'cjkcodecs.shift_jisx0213',
    's_jisx0213'         : 'cjkcodecs.shift_jisx0213',

    # utf_7 codec
    #'utf7'              : 'cjkcodecs.utf_7',
    #'utf_7'             : 'cjkcodecs.utf_7',

    # utf_8 codec
    #'utf8'              : 'cjkcodecs.utf_8',
    #'utf_8'             : 'cjkcodecs.utf_8',
})
del aliases
