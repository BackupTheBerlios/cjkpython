#!/bin/sh
# extractmain.sh: extract CJKCodecs to merge into mainstream Python
# $Id: extracttomain.sh,v 1.4 2004/06/17 18:31:21 perky Exp $

CJKCODECSROOT=..
PYTHONROOT=../../python

PYCJKDIR=$PYTHONROOT/Modules/cjkcodecs
PYENCODINGDIR=$PYTHONROOT/Lib/encodings

# Copy extension module sources
mkdir -p $PYCJKDIR
cp `find $CJKCODECSROOT/src -type f|grep -v CVS` $PYCJKDIR
for f in $PYCJKDIR/*iso_2022*; do
  mv $f `echo $f|sed -e 's,iso_2022,iso2022,g'`
done
for f in $PYCJKDIR/*.*; do
  sed -e 's,^\(#include.*\)maps/\(.*\)$,\1\2,g' \
    -e 's,^\(#include.*\)../\(.*\)$,\1\2,g' \
    -e 's,#include "multibytecodec_compat.h",,g' \
    -e 's,iso_2022,iso2022,g' \
    -e 's,\$Id\(.*\)\$,$CJKCodecs\1$,g' $f |
  python -c 'import re,sys
print re.compile("(\n\s*)*/\*\s*\n\s*\* ex:[^\n]*\n\s*\*/\s*", re.M).sub("", sys.stdin.read())' |
  sed -e '$/^$/d' |
  awk 'BEGIN { output=1; }
/Copyright.*Hye-Shik Chang/ { output=0; }
/\$CJKCodecs/ {
  if (!output)
    print " * Written by Hye-Shik Chang <perky@FreeBSD.org>";
  output=1;
}
{ if (output) print $0; }' > $f.bak
  mv -f $f.bak $f
done
rm -f $PYCJKDIR/_utf_* $PYCJKDIR/multibytecodec_compat.h
unifdef -DPY_VERSION_HEX=0x02040000 -UNO_ERROR_CALLBACKS -UOLD_STYLE_TYPE \
  -UNO_METH_O $PYCJKDIR/multibytecodec.c > $PYCJKDIR/multibytecodec.c.bak 
sed -e 's,OLD_GETATTR_DEF([^)]*),,g' \
  -e 's,GETATTR_FUNC([^)]*),0,g' \
  -e 's,GETATTRO_FUNC([^)]*),PyObject_GenericGetAttr,g' \
  $PYCJKDIR/multibytecodec.c.bak > $PYCJKDIR/multibytecodec.c
rm -f $PYCJKDIR/multibytecodec.c.bak

# Copy pure python libraries
for f in `ls $CJKCODECSROOT/cjkcodecs/*.py |
          grep -v -E '__init__|utf_[78]|aliases'`; do
  DESTPATH=$PYENCODINGDIR/`basename $f`
  DESTPATH=`echo $DESTPATH | sed -e 's,iso_2022,iso2022,g'`
  sed -e 's,^\(#include.*\)maps/\(.*\)$,\1\2,g' \
    -e 's,\$Id\(.*\)\$,$CJKCodecs\1$,g' \
    -e 's,from cjkcodecs\.,from ,g' \
    -e 's,iso_2022,iso2022,g' $f |
  grep -v '^# ex:' |
  sed -e '$/^$/d' |
  awk 'BEGIN { output=1; tignore=0; }
/ACHTUNG/ { tignore=1; }
/Copyright.*Hye-Shik Chang/ { output=0; }
/\$CJKCodecs/ {
  if (!output)
    print "# Written by Hye-Shik Chang <perky@FreeBSD.org>";
  output=1;
}
{
  if (output && !tignore)
    print $0;
  tignore = 0;
}' > $DESTPATH
done

# Copy unittests
for f in $CJKCODECSROOT/tests/test_*.py; do
  DESTPATH=$PYTHONROOT/Lib/test/`basename $f`
  sed -e 's,\$Id\(.*\)\$,$CJKCodecs\1$,g' \
    -e 's,__cjkcodecs__ = 1,__cjkcodecs__ = 0,g' \
    -e "s,encoding = 'cjkcodecs\.,encoding = ',g" \
    -e 's,cjkcodecs\.utf-8,utf-8,g' \
    -e 's,cjkcodecs\.gb18030,gb18030,g' \
    -e 's,iso_2022,iso2022,g' \
    -e 's,^import test_,from test import test_,g' $f |
  sed -e '$/^$/d' |
  grep -v '^# [e]x:' > $DESTPATH
done

# ex: ts=8 sts=2 sw=2 et
