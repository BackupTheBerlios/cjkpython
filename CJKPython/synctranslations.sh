#!/bin/sh
sed -e 's,LANG_ENGLISH,LANG_JAPANESE,g' \
-e 's,English,Japanese,g' text-english.nsh > text-japanese.nsh
sed -e 's,LANG_ENGLISH,LANG_SIMPCHINESE,g' \
-e 's,English,Simplified Chinese,g' text-english.nsh > text-simpchinese.nsh
sed -e 's,LANG_ENGLISH,LANG_TRADCHINESE,g' \
-e 's,English,Traditional Chinese,g' text-english.nsh > text-tradchinese.nsh
unix2dos text-japanese.nsh
unix2dos text-simpchinese.nsh
unix2dos text-tradchinese.nsh
