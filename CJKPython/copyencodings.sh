#!/bin/sh
DESTDIR=`pwd`/../Lib/encodings
cvs -d perky@cvs.sf.net:/cvsroot/python co -d encodings python/dist/src/Lib/encodings
cd encodings
cp cp9*.py big5*.py euc*.py gb*.py hz*.py iso202*.py johab*.py shift_*.py $DESTDIR
cd ..
rm -rf encodings
