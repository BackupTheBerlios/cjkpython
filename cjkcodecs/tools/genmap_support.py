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
# $Id: genmap_support.py,v 1.2 2003/12/31 05:46:55 perky Exp $
#

import re

COPYRIGHT_HEADER = """\
/*
 * $Id: genmap_support.py,v 1.2 2003/12/31 05:46:55 perky Exp $
 */
"""

def printcopyright(fo):
    print >> fo, COPYRIGHT_HEADER

def genmap_encode(codebunch, prefix, emap):
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
        c2map['midx'] = len(codebunch)

        for v in range(rc2values[0], rc2values[-1] + 1):
            if not c2map.has_key(v):
                codebunch.append('NOCHAR,')
            elif isinstance(c2map[v], int):
                codebunch.append('0x%04x,' % c2map[v])
            elif isinstance(c2map[v], tuple):
                codebunch.append('MULTIC,')
            else:
                raise ValueError

def print_encmap(fo, codebunch, fmapprefix, fmap, f2map={}, f2mapprefix=''):
    print >> fo, ("static const DBCHAR __%s_encmap[%d] = {" % (
                        fmapprefix, len(codebunch)))
    i = 0
    while i < len(codebunch):
        dp = codebunch[i:i+8]
        i += 8
        print >> fo, ' ', ' '.join(dp)
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
            print >> fo, "/* 0x%02X */ {0, 0, 0}," % i
            continue

        print >> fo, "/* 0x%02X */ {__%s_encmap+%d, 0x%02x, 0x%02x}," % (
                    i, prefix, map[i]['midx'], map[i]['min'], map[i]['max'])
    print >> fo, "};"
    print >> fo

def genmap_decode(codebunch, prefix, c1range, c2range,
                  dmap, onlymask=(), wide=0):
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
        c2map['midx'] = len(codebunch)

        for v in range(rc2values[0], rc2values[-1] + 1):
            if c2map.has_key(v):
                if not wide:
                    codebunch.append('0x%04x,' % c2map[v])
                else:
                    codebunch.append('0x%08x,' % c2map[v])
            else:
                if not wide:
                    codebunch.append('UNIINV,')
                else:
                    codebunch.append('    UNIINV,')

def print_decmap(fo, codebunch, fmapprefix, fmap, f2map={},
                 f2mapprefix='', wide=0):
    if not wide:
        print >> fo, ("static const ucs2_t __%s_decmap[%d] = {" % (
                        fmapprefix, len(codebunch)))
        width = 8
    else:
        print >> fo, ("static const ucs4_t __%s_decmap[%d] = {" % (
                        fmapprefix, len(codebunch)))
        width = 4
    i = 0
    while i < len(codebunch):
        dp = codebunch[i:i+width]
        i += width
        print >> fo, ' ', ' '.join(dp)
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
            print >> fo, "/* 0x%02X */ {0, 0, 0}," % i
            continue

        print >> fo, "/* 0x%02X */ {__%s_decmap+%d, 0x%02x, 0x%02x}," % (
                    i, prefix, map[i]['midx'], map[i]['min'], map[i]['max'])
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

# ex: ts=8 sts=4 et
