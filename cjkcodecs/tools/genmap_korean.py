#
# genmap_korean.py: Korean Codecs Map Generator
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
# $Id: genmap_korean.py,v 1.5 2004/07/07 15:35:45 perky Exp $
#

from genmap_support import *

KSX1001_C1  = (0x21, 0x7e)
KSX1001_C2  = (0x21, 0x7e)
UHCL1_C1    = (0x81, 0xa0)
UHCL1_C2    = (0x41, 0xfe)
UHCL2_C1    = (0xa1, 0xfe)
UHCL2_C2    = (0x41, 0xa0)

try:
    mapfile = open('CP949.TXT')
except IOError:
    print "=>> Please download mapping table from http://www.unicode." \
          "org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP949.TXT"
    raise SystemExit

print "Loading Mapping File..."
decmap = loadmap(mapfile)
uhcdecmap, ksx1001decmap = {}, {}
cp949encmap = {}
for c1, c2map in decmap.iteritems():
    for c2, code in c2map.iteritems():
        if c1 >= 0xa1 and c2 >= 0xa1:
            ksx1001decmap.setdefault(c1&0x7f, {})
            ksx1001decmap[c1&0x7f][c2&0x7f] = c2map[c2]
            cp949encmap.setdefault(code >> 8, {})
            cp949encmap[code >> 8][code & 0xFF] = (c1<<8 | c2) & 0x7f7f
        else: # uhc
            uhcdecmap.setdefault(c1, {})
            uhcdecmap[c1][c2] = c2map[c2]
            cp949encmap.setdefault(code >> 8, {}) # MSB set
            cp949encmap[code >> 8][code & 0xFF] = (c1<<8 | c2)

omap = open('mappings_kr.h', 'w')
printcopyright(omap)

print "Generating KS X 1001 decode map..."
filler = BufferedFiller()
genmap_decode(filler, "ksx1001", KSX1001_C1, KSX1001_C2, ksx1001decmap)
print_decmap(omap, filler, "ksx1001", ksx1001decmap)

print "Generating UHC decode map..."
filler = BufferedFiller()
genmap_decode(filler, "cp949ext", UHCL1_C1, UHCL1_C2, uhcdecmap)
genmap_decode(filler, "cp949ext", UHCL2_C1, UHCL2_C2, uhcdecmap)
print_decmap(omap, filler, "cp949ext", uhcdecmap)

print "Generating CP949 (includes KS X 1001) encode map..."
filler = BufferedFiller()
genmap_encode(filler, "cp949", cp949encmap)
print_encmap(omap, filler, "cp949", cp949encmap)

print "\nDone!"
