# gencodes: generates python-part codecs.
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
# $Id: gencodecs.py,v 1.6 2004/07/07 17:40:27 perky Exp $
#

codecs = {
    'cn': ('gb2312', 'gbk', 'gb18030', 'hz'),
    'tw': ('big5', 'cp950', 'euc_tw'),
    'hk': ('big5hkscs',),
    'jp': ('cp932', 'shift_jis', 'euc_jp', 'euc_jisx0213', 'shift_jisx0213',
           'euc_jis_2004', 'shift_jis_2004'),
    'kr': ('cp949', 'euc_kr', 'johab'),
    'iso2022': ('iso2022_cn', 'iso2022_jp', 'iso2022_jp_1', 'iso2022_jp_2',
                'iso2022_jp_2004', 'iso2022_jp_3', 'iso2022_jp_ext',
                'iso2022_kr'),
    'unicode': ('utf_7', 'utf_8'),
}
TEMPLATE = 'xxcodec.py.in'

tmpl = open(TEMPLATE).read()
for loc, encodings in codecs.iteritems():
    for enc in encodings:
        code = tmpl.replace('%%encoding%%', enc) \
                .replace('%%ENCODING%%', enc.upper()) \
                .replace('%%locality%%', loc) \
                .replace('%%__%%',
                    'ACHTUNG: This file is generated automatically. '
                    'Please do not edit.')
        open(enc + '.py', 'w').write(code)
