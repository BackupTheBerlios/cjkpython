#
# iconv_codec.py: Universal Unicode Codec for iconv
#
# Copyright (C) 2003 Hye-Shik Chang. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $Id: iconv_codec.py,v 1.1 2003/11/27 09:04:45 perky Exp $

from __future__ import nested_scopes
from _iconv_codec import makeencoder, makedecoder
import codecs, sys

aliases = {}
if sys.platform.startswith('sunos'):
    # sun iconv is case-sensitive (on some platforms) and has too few aliases

    aliases.update({
    # zh_CN
    "cp935": "zh_CN.cp935",
    "euc_cn": "zh_CN.euc",
    "gb18030": "zh_CN.gb18030",
    "gb2312": "zh_CN.euc",
    "gbk": "zh_CN.gbk",
    "hz_gb_2312": "HZ-GB-2312",
    "iso_2022_cn": "zh_CN.iso2022-CN",

    # zh_TW
    "big5": "zh_TW.big5",
    "big5p": "zh_TW.big5p",
    "cp937": "zh_TW.cp937",
    "euc_tw": "zh_TW.euc",
    "iso_2022_tw": "zh_TW.iso2022-7",

    # zh_HK
    "big5hk": "zh_HK.hkscs",
    "hkscs": "zh_HK.hkscs",

    # japanese
    "iso_2022_jp": "ISO-2022-JP",
    "euc_jp": "eucJP",
    "shift_jis": "SJIS",

    # korean
    "euc_kr": "ko_KR.euc",
    "iso_2022_kr": "ko_KR.iso2022-7",
    "johab": "ko_KR.johap",
    "johab92": "ko_KR.johap92",
    "cp932": "ko_KR.cp932",
    "cp949": "ko_KR.cp949",
    })

def lookup(enc):
    if enc.startswith('iconvcodec.'):
        enc = enc[11:]
    enc = aliases.get(enc.replace('-', '_'), enc)
    try:
        encoder, decoder = makeencoder(enc), makedecoder(enc)
    except (LookupError, RuntimeError):
        enc = enc.replace('_', '-')
        try:
            encoder, decoder = makeencoder(enc), makedecoder(enc)
        except (LookupError, RuntimeError):
            return None

    class IconvCodec(codecs.Codec):
        encode = encoder
        decode = decoder

    class IconvStreamReader(IconvCodec, codecs.StreamReader):
        def __init__(self, stream, errors='strict'):
            codecs.StreamReader.__init__(self, stream, errors)
            __codec = self.decode.makestream(stream, errors)
            self.read = __codec.read
            self.readline = __codec.readline
            self.readlines = __codec.readlines
            self.reset = __codec.reset

    class IconvStreamWriter(IconvCodec, codecs.StreamWriter):
        def __init__(self, stream, errors='strict'):
            codecs.StreamWriter.__init__(self, stream, errors)
            __codec = self.encode.makestream(stream, errors)
            self.write = __codec.write
            self.writelines = __codec.writelines
            self.reset = __codec.reset

    return (encoder, decoder, IconvStreamReader, IconvStreamWriter)

codecs.register(lookup)

# ex: ts=8 sts=4 et
