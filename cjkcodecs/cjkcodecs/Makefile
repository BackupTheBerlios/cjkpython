# Makefile: maintenance tool set
#
# Copyright (C) 2003 Hye-Shik Chang <perky@FreeBSD.org>.
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
# $Id: Makefile,v 1.3 2003/11/29 23:07:32 perky Exp $
#

GENERIC_ENCODINGS=	gb2312 gbk gb18030 hz \
			big5 cp950 \
			cp932 shift_jis euc_jp \
			iso_2022_jp iso_2022_jp_1 iso_2022_jp_2 \
			iso_2022_jp_3 euc_jisx0213 shift_jisx0213 \
			shift_jis_strict euc_jp_strict \
			cp949 euc_kr johab iso_2022_kr \
			utf_7 utf_8

all:
	for cset in ${GENERIC_ENCODINGS}; do \
		CSET=`echo $$cset|tr "[:lower:]" "[:upper:]"`; \
		if [ ! -f $$cset.py ]; then \
		sed -e "s/%%ENCODING%%/$$CSET/g" \
		-e "s/%%encoding%%/$$cset/g" \
		-e "s/%%__%%/ACHTUNG: This file is generated automatically.\
		Please do not edit./g" xxcodec.py.in \
			> $$cset.py; \
		fi \
	done
