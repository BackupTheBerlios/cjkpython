#
# genmap_tchinese.py: Traditional Chinese Codecs Map Generator
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
# $Id: genmap_tchinese.py,v 1.2 2003/12/31 05:46:55 perky Exp $
#

from genmap_support import *

BIG5_C1     = (0xa1, 0xfe)
BIG5_C2     = (0x40, 0xfe)
# big5 map doesn't have 0xA3E1 (EURO SIGN), but we ignore
# that for forward compatiblilty. "Hey! we have the euro-big5!" :)
CP950_C1    = BIG5_C1
CP950_C2    = BIG5_C2

try:
    big5map = open('BIG5.TXT')
except IOError:
    print "=>> Please download mapping table from http://www.unicode." \
          "org/Public/MAPPINGS/OBSOLETE/EASTASIA/OTHER/BIG5.TXT"
    raise SystemExit
try:
    cp950map = open('CP950.TXT')
except IOError:
    print "=>> Please download mapping table from http://www.unicode." \
          "org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP950.TXT"
    raise SystemExit

print "Loading Mapping File..."
cp950decmap = loadmap(cp950map)
big5decmap = loadmap(big5map)

# big5 mapping fix (see doc/NOTES.big5)
for m in """\
0xA15A      0x2574
0xA1C3      0xFFE3
0xA1C5      0x02CD
0xA1FE      0xFF0F
0xA240      0xFF3C
0xA2CC      0x5341
0xA2CE      0x5345""".splitlines():
    bcode, ucode = map(eval, m.split())
    big5decmap[bcode >> 8][bcode & 0xff] = ucode

big5encmap, cp950encmap = {}, {}
for c1, m in big5decmap.items():
    for c2, code in m.items():
        big5encmap.setdefault(code >> 8, {})
        if not big5encmap[code >> 8].has_key(code & 0xff):
            big5encmap[code >> 8][code & 0xff] = c1 << 8 | c2
for c1, m in cp950decmap.items():
    for c2, code in m.items():
        cp950encmap.setdefault(code >> 8, {})
        if not cp950encmap[code >> 8].has_key(code & 0xff):
            cp950encmap[code >> 8][code & 0xff] = c1 << 8 | c2

# fix unicode->big5 duplicated mapping priority
big5encmap[0xFF][0x0F] = 0xA241
big5encmap[0xFF][0x3C] = 0xA242
big5encmap[0x53][0x41] = 0xA451
big5encmap[0x53][0x45] = 0xA4CA
cp950encmap[0x53][0x41] = 0xA451
cp950encmap[0x53][0x45] = 0xA4CA

for c1, m in cp950encmap.items():
    for c2, code in m.items():
        if (big5encmap.has_key(c1) and big5encmap[c1].has_key(c2)
                and big5encmap[c1][c2] == code):
            del cp950encmap[c1][c2]
for c1, m in cp950decmap.items():
    for c2, code in m.items():
        if (big5decmap.has_key(c1) and big5decmap[c1].has_key(c2)
                and big5decmap[c1][c2] == code):
            del cp950decmap[c1][c2]

omap = open('map_big5.h', 'w')
printcopyright(omap)

print "Generating BIG5 decode map..."
codebunch = []
genmap_decode(codebunch, "big5", BIG5_C1, BIG5_C2, big5decmap)
print_decmap(omap, codebunch, "big5", big5decmap)

print "Generating BIG5 encode map..."
codebunch = []
genmap_encode(codebunch, "big5", big5encmap)
print_encmap(omap, codebunch, "big5", big5encmap)

omap = open('map_cp950ext.h', 'w')
printcopyright(omap)

print "Generating CP950 extension decode map..."
codebunch = []
genmap_decode(codebunch, "cp950ext", BIG5_C1, BIG5_C2, cp950decmap)
print_decmap(omap, codebunch, "cp950ext", cp950decmap)

print "Generating CP950 extension encode map..."
codebunch = []
genmap_encode(codebunch, "cp950ext", cp950encmap)
print_encmap(omap, codebunch, "cp950ext", cp950encmap)

print "\nDone!"
# ex: ts=8 sts=4 et
