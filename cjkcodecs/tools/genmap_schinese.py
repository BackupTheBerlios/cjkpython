#
# genmap_schinese.py: Simplified Chinese Codecs Map Generator
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
# $Id: genmap_schinese.py,v 1.5 2004/07/07 15:35:45 perky Exp $
#

from genmap_support import *

GB2312_C1   = (0x21, 0x7e)
GB2312_C2   = (0x21, 0x7e)
GBKL1_C1    = (0x81, 0xa8)
GBKL1_C2    = (0x40, 0xfe)
GBKL2_C1    = (0xa9, 0xfe)
GBKL2_C2    = (0x40, 0xa0)
GB18030EXTP1_C1 = (0xa1, 0xa9)
GB18030EXTP1_C2 = (0x40, 0xfe)
GB18030EXTP2_C1 = (0xaa, 0xaf)
GB18030EXTP2_C2 = (0xa1, 0xfe)
GB18030EXTP3_C1 = (0xd7, 0xd7)
GB18030EXTP3_C2 = (0xfa, 0xfe)
GB18030EXTP4_C1 = (0xf8, 0xfd)
GB18030EXTP4_C2 = (0xa1, 0xfe)
GB18030EXTP5_C1 = (0xfe, 0xfe)
GB18030EXTP5_C2 = (0x50, 0xfe)


try:
    gb2312map = open('GB2312.TXT')
except IOError:
    print "=>> Please download mapping table from http://www.unicode." \
          "org/Public/MAPPINGS/OBSOLETE/EASTASIA/GB/GB2312.TXT"
    raise SystemExit
try:
    cp936map = open('CP936.TXT')
except IOError:
    print "=>> Please download mapping table from http://www.unicode." \
          "org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP936.TXT"
    raise SystemExit
try:
    gb18030map = open('gb-18030-2000.xml')
except IOError:
    print "=>> Please download mapping table from http://oss.software" \
          ".ibm.com/cvs/icu/~checkout~/charset/data/xml/gb-18030-2000.xml"
    raise SystemExit

re_gb18030ass = re.compile('<a u="([A-F0-9]{4})" b="([0-9A-F ]+)"/>')
def parse_gb18030map(fo):
    m, gbuni = {}, {}
    for i in range(65536):
        if i < 0xd800 or i > 0xdfff: # exclude unicode surrogate area
            gbuni[i] = None
    for uni, native in re_gb18030ass.findall(fo.read()):
        uni = eval('0x'+uni)
        native = [eval('0x'+u) for u in native.split()]
        if len(native) <= 2:
            del gbuni[uni]
        if len(native) == 2: # we can decode algorithmically for 1 or 4 bytes
            m.setdefault(native[0], {})
            m[native[0]][native[1]] = uni
    gbuni = gbuni.keys()
    gbuni.sort()
    return m, gbuni

print "Loading Mapping File..."
gb18030decmap, gb18030unilinear = parse_gb18030map(gb18030map)
gbkdecmap = loadmap(cp936map)
gb2312decmap = loadmap(gb2312map)
difmap = {}
for c1, m in gbkdecmap.items():
    for c2, code in m.items():
        del gb18030decmap[c1][c2]
        if not gb18030decmap[c1]:
            del gb18030decmap[c1]
for c1, m in gb2312decmap.items():
    for c2, code in m.items():
        gbkc1, gbkc2 = c1 | 0x80, c2 | 0x80
        if gbkdecmap[gbkc1][gbkc2] == code:
            del gbkdecmap[gbkc1][gbkc2]
            if not gbkdecmap[gbkc1]:
                del gbkdecmap[gbkc1]

gb2312_gbkencmap, gb18030encmap = {}, {}
for c1, m in gbkdecmap.iteritems():
    for c2, code in m.iteritems():
        gb2312_gbkencmap.setdefault(code >> 8, {})
        gb2312_gbkencmap[code >> 8][code & 0xff] = c1 << 8 | c2 # MSB set
for c1, m in gb2312decmap.iteritems():
    for c2, code in m.iteritems():
        gb2312_gbkencmap.setdefault(code >> 8, {})
        gb2312_gbkencmap[code >> 8][code & 0xff] = c1 << 8 | c2 # MSB unset
for c1, m in gb18030decmap.iteritems():
    for c2, code in m.iteritems():
        gb18030encmap.setdefault(code >> 8, {})
        gb18030encmap[code >> 8][code & 0xff] = c1 << 8 | c2

omap = open('mappings_cn.h', 'w')
printcopyright(omap)

print "Generating GB2312 decode map..."
filler = BufferedFiller()
genmap_decode(filler, "gb2312", GB2312_C1, GB2312_C2, gb2312decmap)
print_decmap(omap, filler, "gb2312", gb2312decmap)

print "Generating GBK decode map..."
filler = BufferedFiller()
genmap_decode(filler, "gbkext", GBKL1_C1, GBKL1_C2, gbkdecmap)
genmap_decode(filler, "gbkext", GBKL2_C1, GBKL2_C2, gbkdecmap)
print_decmap(omap, filler, "gbkext", gbkdecmap)

print "Generating GB2312 && GBK encode map..."
filler = BufferedFiller()
genmap_encode(filler, "gbcommon", gb2312_gbkencmap)
print_encmap(omap, filler, "gbcommon", gb2312_gbkencmap)

print "Generating GB18030 extension decode map..."
filler = BufferedFiller()
for i in range(1, 6):
    genmap_decode(filler, "gb18030ext", eval("GB18030EXTP%d_C1" % i),
                    eval("GB18030EXTP%d_C2" % i), gb18030decmap)
print_decmap(omap, filler, "gb18030ext", gb18030decmap)

print "Generating GB18030 extension encode map..."
filler = BufferedFiller()
genmap_encode(filler, "gb18030ext", gb18030encmap)
print_encmap(omap, filler, "gb18030ext", gb18030encmap)

print "Generating GB18030 Unicode BMP Mapping Ranges..."
ranges = [[-1, -1, -1]]
gblinnum = 0
print >> omap, """
static const struct _gb18030_to_unibmp_ranges {
    Py_UNICODE   first, last;
    DBCHAR       base;
} gb18030_to_unibmp_ranges[] = {"""
for uni in gb18030unilinear:
    if uni == ranges[-1][1] + 1:
        ranges[-1][1] = uni
    else:
        ranges.append([uni, uni, gblinnum])
    gblinnum += 1
filler = BufferedFiller()
for first, last, base in ranges[1:]:
    filler.write('{', str(first), ',', str(last), ',', str(base), '},')
filler.write('{', '0,', '0,', str(
    ranges[-1][2] + ranges[-1][1] - ranges[-1][0] + 1), '}', '};')
filler.printout(omap)

print "\nDone!"
