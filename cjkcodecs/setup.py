#!/usr/bin/env python
#
# setup.py: CJK Codec Setup
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
# $Id: setup.py,v 1.6 2003/11/27 15:32:00 perky Exp $
#

import os, shutil
import sys
from distutils.core import setup, Extension
from distutils.command.install import install

SRCDIR = './src'
TMPSRCDIR = './build/tmpsrc'

LIBDIRS = []
extensions = []
encodings = {
'ja_JP':    ['shift_jis', 'cp932', 'euc_jp', 'iso_2022_jp', 'iso_2022_jp_1',
             'iso_2022_jp_2', 'iso_2022_jp_3', 'shift_jisx0213',
             'euc_jisx0213'],
'ko_KR':    ['euc_kr', 'cp949', 'johab', 'iso_2022_kr'],
'zh_CN':    ['gb2312', 'gbk', 'gb18030', 'hz'],
'zh_TW':    ['big5', 'cp950'],
'':         ['utf_7', 'utf_8'],
}
locales = encodings.keys()
strictencodings = (
'shift_jis', 'euc_jp', 'euc_jisx0213',
)

for arg in sys.argv[1:]: # don't use getopt to ignore arguments for distutils
    args = arg.split('=', 1)
    if args[0] == '--disable-japanese':
        locales.remove('ja_JP')
    elif args[0] == '--disable-korean':
        locales.remove('ko_KR')
    elif args[0] == '--disable-simplified-chinese':
        locales.remove('zh_CN')
    elif args[0] == '--disable-traditional-chinese':
        locales.remove('zh_TW')
    elif args[0] == '--disable-utf':
        locales.remove('')
    elif args[0] == '--help':
        print """\
Language options:
  --disable-japanese                don't install Japanese codecs
  --disable-korean                  don't install Korean codecs
  --disable-simplified-chinese      don't install Simplified Chinese codecs
  --disable-traditional-chinese     don't install Traditional Chinese codecs
  --disable-utf                     don't install UTF codecs
"""
        continue
    else:
        continue

    sys.argv.remove(arg)

if sys.platform == 'win32' and '--compiler=mingw32' in sys.argv:
    LIBDIRS.append('.') # libpython23.a and libpython23.def

try:
    os.makedirs(TMPSRCDIR)
except OSError, why:
    import errno
    if why.errno != errno.EEXIST:
        raise OSError, why

for loc in locales:
    if loc:
        extensions.append(Extension('cjkcodecs.mapdata_'+loc,
                    ['%s/maps/mapdata_%s.c' % (SRCDIR, loc)]))
    for enc in encodings[loc]:
        extensions.append(Extension('cjkcodecs._'+enc,
                          ['%s/_%s.c' % (SRCDIR, enc)], library_dirs=LIBDIRS))
        if enc in strictencodings:
            shutil.copy('%s/_%s.c' % (SRCDIR, enc),
                        '%s/_%s_strict.c' % (TMPSRCDIR, enc))
            extensions.append(Extension('cjkcodecs._'+enc+'_strict',
                ['%s/_%s_strict.c' % (TMPSRCDIR, enc)],
                include_dirs=[SRCDIR], library_dirs=LIBDIRS,
                define_macros=[('STRICT_BUILD', 1)]))


class Install(install):
    def initialize_options (self):
        install.initialize_options(self)
        if sys.hexversion >= 0x2010000:
            self.extra_path = ("cjkcodecs", "import cjkcodecs.aliases")
        else:
            self.extra_path = "cjkcodecs"
    def finalize_options (self):
        org_install_lib = self.install_lib
        install.finalize_options(self)
        self.install_libbase = self.install_lib = \
            org_install_lib or self.install_purelib

setup (name = "cjkcodecs",
       version = "1.0.2",
       description = "Python Unicode Codecs Collection for CJK Encodings",
       author = "Hye-Shik Chang",
       author_email = "perky@FreeBSD.org",
       url = "http://sourceforge.net/projects/koco",
       license = "BSD Style (without advertisement clause)",
       cmdclass = {'install': Install},
       packages = ['cjkcodecs'],
       ext_modules =
            [Extension("cjkcodecs.multibytecodec",
                       ["%s/multibytecodec.c" % SRCDIR], library_dirs=LIBDIRS)]
            + extensions
       )

# ex: ts=8 sts=4 et
