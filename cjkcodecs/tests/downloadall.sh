#!/bin/sh
# $Id: downloadall.sh,v 1.2 2003/12/19 02:39:09 perky Exp $

MAPURLS=""
for locale in cn jp kr tw; do
	MAPURL=`python -c "
import test_codecmaps_$locale as cm
print '\n'.join([
	getattr(cm, cl).mapfileurl for cl in dir(cm)
	if cl.startswith('Test')
])"`
	MAPURLS="$MAPURLS $MAPURL"
done

wget -nc $MAPURLS
