#!/bin/sh
# $Id: distpatch.sh,v 1.1 2003/09/23 11:13:42 perky Exp $

# fix CR CR LF wrong line terminators
dos2unix dist/wxPython/wxPython/tools/XRCed/params.py
unix2dos dist/wxPython/wxPython/tools/XRCed/params.py
dos2unix dist/wxPython/wxPython/tools/XRCed/tree.py
unix2dos dist/wxPython/wxPython/tools/XRCed/tree.py
dos2unix dist/wxPython/wxPython/tools/XRCed/xxx.py
unix2dos dist/wxPython/wxPython/tools/XRCed/xxx.py

# convert LF to CR LF
unix2dos dist/KoreanCodecs-pure-1.0b1/korean/hangul.py
