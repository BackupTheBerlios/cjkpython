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
# $Id: genmap_tchinese.py,v 1.6 2004/06/28 16:50:37 perky Exp $
#

import sys
import re
from genmap_support import *

BIG5_C1     = (0xa1, 0xfe)
BIG5_C2     = (0x40, 0xfe)
# big5 map doesn't have 0xA3E1 (EURO SIGN), but we ignore
# that for forward compatiblilty. "Hey! we have the euro-big5!" :)
CP950_C1    = BIG5_C1
CP950_C2    = BIG5_C2
BIG5HKSCS_C1= (0x88, 0xfe)
BIG5HKSCS_C2= (0x40, 0xfe)
CNS11643_C1 = (0x21, 0x7e)
CNS11643_C2 = (0x21, 0x7e)

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
try:
    big5hkscsmap = open('big5-iso.txt')
except IOError:
    print "=>> Please download mapping table from http://www.info.gov." \
          "hk/digital21/eng/hkscs/download/big5-iso.txt"
    raise SystemExit

try:
    cns11643map = open("cns-11643-1992.ucm")
except IOError:
    print "=>> Please download mapping table from http://oss.software." \
          "ibm.com/cvs/icu/~checkout~/icu/source/data/mappings/cns-11643" \
          "-1992.ucm"
    raise SystemExit

class CNS11643MapReader(UCMReader):
    def parsedata(self, data):
        d = UCMReader.parsedata(self, data)
        return ord(d[0]) - 0x80, (ord(d[1]) << 8) + ord(d[2])
    def readmap(self):
        print "Loading UCM mapping from ", self.file
        decmaps = [{} for i in range(8)] # 0(placeholder) 1-7
        encmap_bmp = {}
        encmap_nonbmp = {}
        for i, (plane, code) in self.itertokens():
            if code & 0x8000: raise ValueError
            if i < 0x10000:
                c1i = code >> 8
                encmap = encmap_bmp
            elif 0x20000 <= i < 0x30000:
                c1i = (code >> 8) | 0x80
                i &= 0xffff
                encmap = encmap_nonbmp
            else:
                raise ValueError, i
            decmaps[plane].setdefault(c1i, {})
            decmaps[plane][c1i][code & 0xff] = i
            encmap.setdefault(i >> 8, {})
            encmap[i >> 8][i & 0xff] = (plane, code)
        return decmaps, encmap_bmp, encmap_nonbmp

def bh2s(code):
    return ((code >> 8) - 0x88) * (0xfe - 0x40 + 1) + ((code & 0xff) - 0x40)

def loadhkscsmap(fo):
    print "Loading from", fo
    fo.seek(0, 0)
    decmap = {}
    encmap_bmp, encmap_nonbmp = {}, {}
    isbmpmap = {}
    lineparse = re.compile('^([0-9A-F]{4})\s*([0-9A-F]{4})\s*([0-9A-F]{4})\s*'
                           '(2?[0-9A-F]{4})$')
    for line in fo:
        data = lineparse.findall(line)
        if not data:
            continue
        hkscs, u1993, ubmp, u2001 = [int(x, 16) for x in data[0]]
        decmap.setdefault(hkscs >> 8, {})
        if u2001 > 0xffff:
            encmap = encmap_nonbmp
            isbmpmap[bh2s(hkscs)] = 1
            u2001 &= 0xffff
        else:
            encmap = encmap_bmp
        decmap[hkscs >> 8][hkscs & 0xff] = u2001
        encmap.setdefault(u2001 >> 8, {})
        encmap[u2001 >> 8][u2001 & 0xff] = hkscs

    return decmap, encmap_bmp, encmap_nonbmp, isbmpmap

print "Loading Mapping File..."
cp950decmap = loadmap(cp950map)
big5decmap = loadmap(big5map)
hkscsdecmap, hkscsencmap_bmp, hkscsencmap_nonbmp, isbmpmap = \
    loadhkscsmap(big5hkscsmap)
cns11643decmaps, cns11643encmap_bmp, cns11643encmap_nonbmp = \
    CNS11643MapReader(cns11643map).readmap()

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
filler = BufferedFiller()
genmap_decode(filler, "big5", BIG5_C1, BIG5_C2, big5decmap)
print_decmap(omap, filler, "big5", big5decmap)

print "Generating BIG5 encode map..."
filler = BufferedFiller()
genmap_encode(filler, "big5", big5encmap)
print_encmap(omap, filler, "big5", big5encmap)

omap = open('map_cp950ext.h', 'w')
printcopyright(omap)

print "Generating CP950 extension decode map..."
filler = BufferedFiller()
genmap_decode(filler, "cp950ext", BIG5_C1, BIG5_C2, cp950decmap)
print_decmap(omap, filler, "cp950ext", cp950decmap)

print "Generating CP950 extension encode map..."
filler = BufferedFiller()
genmap_encode(filler, "cp950ext", cp950encmap)
print_encmap(omap, filler, "cp950ext", cp950encmap)

omap = open('map_big5hkscs.h', 'w')
printcopyright(omap)

print "Generating BIG5HKSCS decode map..."
filler = BufferedFiller()
genmap_decode(filler, "big5hkscs", BIG5HKSCS_C1, BIG5HKSCS_C2, hkscsdecmap)
print_decmap(omap, filler, "big5hkscs", hkscsdecmap)

print "Generating BIG5HKSCS decode map Unicode plane hints..."
filler = BufferedFiller()
def fillhints(hintfrom, hintto):
    print >> omap, "static const unsigned char big5hkscs_phint_%d[] = {" \
                    % (hintfrom)
    for msbcode in range(hintfrom, hintto+1, 8):
        v = 0
        for c in range(msbcode, msbcode+8):
            v = (v << 1) | isbmpmap.get(c, 0)
        filler.write('%d,' % v)
    filler.printout(omap)
    print >> omap, "};\n"
fillhints(bh2s(0x8840), bh2s(0xa0fe))
fillhints(bh2s(0xc6a1), bh2s(0xc8fe))
fillhints(bh2s(0xf9d6), bh2s(0xfefe))

print "Generating BIG5HKSCS encode map (BMP)..."
filler = BufferedFiller()
genmap_encode(filler, "big5hkscs_bmp", hkscsencmap_bmp)
print_encmap(omap, filler, "big5hkscs_bmp", hkscsencmap_bmp)

print "Generating BIG5HKSCS encode map (non-BMP)..."
filler = BufferedFiller()
genmap_encode(filler, "big5hkscs_nonbmp", hkscsencmap_nonbmp)
print_encmap(omap, filler, "big5hkscs_nonbmp", hkscsencmap_nonbmp)

omap = open('map_cns11643.h', 'w')
printcopyright(omap)
for plane in range(1, 8):
    print "Generating CNS11643 plane %d decode map..." % plane
    charset = "cns11643_%d" % plane
    filler = BufferedFiller()
    genmap_decode(filler, charset, CNS11643_C1, CNS11643_C2,
                  cns11643decmaps[plane])
    genmap_decode(filler, charset, [x | 0x80 for x in CNS11643_C1],
                  CNS11643_C2, cns11643decmaps[plane])
    print_decmap(omap, filler, charset, cns11643decmaps[plane])

class CNS11643EncodeMapWriter(EncodeMapWriter):
    elemtype = 'unsigned char'
    indextype = 'struct unim_index_bytebased'
    def write_nochar(self):
        self.filler.write('0,', '0,', '0,')
    def write_char(self, point):
        raise ValueError
    def write_multic(self, point):
        self.filler.write('%d,' % point[0], '%d,' % (point[1] >> 8),
                          '%d,' % (point[1] & 0xff))

print "Generating CNS11643 encode map (BMP)..."
CNS11643EncodeMapWriter(omap, 'cns11643_bmp', cns11643encmap_bmp)
print "Generating CNS11643 encode map (non-BMP)..."
CNS11643EncodeMapWriter(omap, 'cns11643_nonbmp', cns11643encmap_nonbmp)
print >> omap, "static const struct dbcs_index *cns11643_decmap[] = {"
print >> omap, "NULL,"
print >> omap, "\n".join(["cns11643_%d_decmap," % plane
                        for plane in range(1, 8)])
print >> omap, "};"

print "\nDone!"
