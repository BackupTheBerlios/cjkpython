#!/bin/sh
# extractmain.sh: extract CJKCodecs to merge into mainstream Python
# $Id: extracttomain.sh,v 1.1 2004/01/05 05:51:40 perky Exp $

CJKCODECSROOT=..
PYTHONROOT=../../python

PYCJKDIR=$PYTHONROOT/Modules/cjkcodecs

mkdir -p $PYCJKDIR
cp `find $CJKCODECSROOT/src -type f|grep -v CVS` $PYCJKDIR
for f in $PYCJKDIR/*.*; do
  sed -e 's,^\(#include.*\)maps/\(.*\)$,\1\2,g' \
    -e 's,\$Id\(.*\)\$,$CJKCodecs\1$,g' $f |
  awk 'BEGIN { output=1; }
/Copyright.*Hye-Shik Chang/ { output=0; }
/\$CJKCodecs/ {
  print " * Written by Hye-Shik Chang <perky@FreeBSD.org>";
  output=1;
}
{
  if (output)
    print $0;
}' > $f.bak
  mv -f $f.bak $f
done
rm -f $PYCJKDIR/_utf_* $PYCJKDIR/multibytecodec_compat.h
unifdef -DPY_VERSION_HEX=0x02040000 -UNO_ERROR_CALLBACKS -UOLD_STYLE_TYPE \
  -UNO_METH_O $PYCJKDIR/multibytecodec.c > $PYCJKDIR/multibytecodec.c.bak 
sed -e 's,OLD_GETATTR_DEF([^)]*),,g' \
  -e 's,GETATTR_FUNC([^)]*),0,g' \
  -e 's,GETATTRO_FUNC([^)]*),PyObject_GenericGetAttr,g' \
  -e 's,#include "multibytecodec_compat.h",,g' \
  $PYCJKDIR/multibytecodec.c.bak > $PYCJKDIR/multibytecodec.c
rm -f $PYCJKDIR/multibytecodec.c.bak

# ex: ts=8 sts=2 sw=2 et
