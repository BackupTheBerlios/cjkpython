#!/bin/sh
# $Id: downloadall.sh,v 1.1 2003/09/24 17:47:01 perky Exp $
python testall.py|sed -n -e 's,^.*\(http[^ ]*\).*$,\1,p'|xargs -n1 wget
