#
# genmap_support.py: Multibyte Codec Map Generator
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
# $Id: genmap_support.py,v 1.4 2004/06/19 06:11:46 perky Exp $
#

import re

COPYRIGHT_HEADER = """\
/*
 * $Id: genmap_support.py,v 1.4 2004/06/19 06:11:46 perky Exp $
 */
"""

class BufferedFiller:
    def __init__(self, column=78):
        self.column = column
        self.buffered = []
        self.cline = []
        self.clen = 0
        self.count = 0
    def write(self, *data):
        for s in data:
            if len(s) > self.column:
                raise ValueError, "token is too long"
            if len(s) + self.clen > self.column:
                self.flush()
            self.clen += len(s)
            self.cline.append(s)
            self.count += 1
    def flush(self):
        if not self.cline:
            return
        self.buffered.append(''.join(self.cline))
        self.clen = 0
        del self.cline[:]
    def printout(self, fp):
        self.flush()
        for l in self.buffered:
            print >> fp, l
        del self.buffered[:]
    def __len__(self):
        return self.count

def printcopyright(fo):
    print >> fo, COPYRIGHT_HEADER

def genmap_encode(filler, prefix, emap):
    for c1 in range(0, 256):
        if not emap.has_key(c1):
            continue
        c2map = emap[c1]
        rc2values = c2map.keys()
        rc2values.sort()
        if not rc2values:
            continue

        c2map[prefix] = True
        c2map['min'] = rc2values[0]
        c2map['max'] = rc2values[-1]
        c2map['midx'] = len(filler)

        for v in range(rc2values[0], rc2values[-1] + 1):
            if not c2map.has_key(v):
                filler.write('N,')
            elif isinstance(c2map[v], int):
                filler.write(str(c2map[v]) + ',')
            elif isinstance(c2map[v], tuple):
                filler.write('M,')
            else:
                raise ValueError

def print_encmap(fo, filler, fmapprefix, fmap, f2map={}, f2mapprefix=''):
    print >> fo, ("static const DBCHAR __%s_encmap[%d] = {" % (
                        fmapprefix, len(filler)))
    filler.printout(fo)
    print >> fo, "};"
    print >> fo

    print >> fo, "static const struct unim_index %s_encmap[256] = {" % (fmapprefix)
    for i in range(256):
        if fmap.has_key(i) and fmap[i].has_key(fmapprefix):
            map = fmap
            prefix = fmapprefix
        elif f2map.has_key(i) and f2map[i].has_key(f2mapprefix):
            map = f2map
            prefix = f2mapprefix
        else:
            filler.write("{", "0,", "0,", "0", "},")
            continue

        filler.write("{", "__%s_encmap" % prefix, "+", "%d" % map[i]['midx'],
                     ",", "%d," % map[i]['min'], "%d," % map[i]['max'], "},")
    filler.printout(fo)
    print >> fo, "};"
    print >> fo

def genmap_decode(filler, prefix, c1range, c2range, dmap, onlymask=(),
                  wide=0):
    c2width  = c2range[1] - c2range[0] + 1
    c2values = range(c2range[0], c2range[1] + 1)

    for c1 in range(c1range[0], c1range[1] + 1):
        if not dmap.has_key(c1) or (onlymask and c1 not in onlymask):
            continue
        c2map = dmap[c1]
        rc2values = [n for n in c2values if c2map.has_key(n)]
        if not rc2values:
            continue

        c2map[prefix] = True
        c2map['min'] = rc2values[0]
        c2map['max'] = rc2values[-1]
        c2map['midx'] = len(filler)

        for v in range(rc2values[0], rc2values[-1] + 1):
            if c2map.has_key(v):
                filler.write('%d,' % c2map[v])
            else:
                filler.write('U,')

def print_decmap(fo, filler, fmapprefix, fmap, f2map={}, f2mapprefix='',
                 wide=0):
    if not wide:
        print >> fo, ("static const ucs2_t __%s_decmap[%d] = {" % (
                        fmapprefix, len(filler)))
        width = 8
    else:
        print >> fo, ("static const ucs4_t __%s_decmap[%d] = {" % (
                        fmapprefix, len(filler)))
        width = 4
    filler.printout(fo)
    print >> fo, "};"
    print >> fo

    if not wide:
        print >> fo, "static const struct dbcs_index %s_decmap[256] = {" % (
                        fmapprefix)
    else:
        print >> fo, "static const struct widedbcs_index %s_decmap[256] = {" % (
                        fmapprefix)

    for i in range(256):
        if fmap.has_key(i) and fmap[i].has_key(fmapprefix):
            map = fmap
            prefix = fmapprefix
        elif f2map.has_key(i) and f2map[i].has_key(f2mapprefix):
            map = f2map
            prefix = f2mapprefix
        else:
            filler.write("{", "0,", "0,", "0", "},")
            continue

        filler.write("{", "__%s_decmap" % prefix, "+", "%d" % map[i]['midx'],
                     ",", "%d," % map[i]['min'], "%d," % map[i]['max'], "},")
    filler.printout(fo)
    print >> fo, "};"
    print >> fo

def loadmap(fo, natcol=0, unicol=1, sbcs=0):
    print "Loading from", fo
    fo.seek(0, 0)
    decmap = {}
    for line in fo:
        line = line.split('#', 1)[0].strip()
        if not line or len(line.split()) < 2: continue

        row = map(eval, line.split())
        loc, uni = row[natcol], row[unicol]
        if loc >= 0x100 or sbcs:
            decmap.setdefault((loc >> 8), {})
            decmap[(loc >> 8)][(loc & 0xff)] = uni

    return decmap
