#!/usr/bin/env python
#
# setup.py: CJK Codec Setup
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
# $Id: setup.py,v 1.18 2004/06/28 18:16:03 perky Exp $
#

import os
import sys
from distutils.core import setup, Extension
from distutils.command.install import install

SRCDIR = 'src'
LIBDIRS = []
extensions = []
macros = []
locales = ['kr', 'jp', 'cn', 'tw', 'hk', 'iso2022', 'unicode']

for arg in sys.argv[1:]: # don't use getopt to ignore arguments for distutils
    args = arg.split('=', 1)
    if args[0] == '--disable-korean':
        locales.remove('kr')
    elif args[0] == '--disable-japanese':
        locales.remove('jp')
    elif args[0] == '--disable-simplified-chinese':
        locales.remove('cn')
    elif args[0] == '--disable-traditional-chinese':
        locales.remove('tw')
        locales.remove('hk')
    elif args[0] == '--disable-iso2022':
        locales.remove('iso2022')
    elif args[0] == '--disable-utf':
        locales.remove('unicode')
    elif args[0] == '--disable-extra-encodings':
        macros.append(('NO_EXTRA_ENCODINGS', '1'))
    elif args[0] == '--help':
        print """\
Language options:
  --disable-japanese                don't install Japanese codecs
  --disable-korean                  don't install Korean codecs
  --disable-simplified-chinese      don't install Simplified Chinese codecs
  --disable-traditional-chinese     don't install Traditional Chinese codecs
  --disable-iso2022                 don't install ISO-2022 codecs
  --disable-utf                     don't install UTF codecs
  --disable-extra-encodings         disable building extra-expensive encodings:
                                        euc-tw iso-2022-cn iso-2022-ext
"""
        continue
    else:
        continue

    sys.argv.remove(arg)

if sys.platform == 'win32' and '--compiler=mingw32' in sys.argv:
    LIBDIRS.append('.') # libpython23.a and libpython23.def

for loc in locales:
    extensions.append(Extension('cjkcodecs._codecs_' + loc,
                      ['%s/_codecs_%s.c' % (SRCDIR, loc)],
                      define_macros=macros))

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
       version = "1.1",
       description = "Python Unicode Codecs Collection for CJK Encodings",
       author = "Hye-Shik Chang",
       author_email = "perky@FreeBSD.org",
       url = "http://cjkpython.i18n.org/#CJKCodecs",
       license = "BSD Style (without advertisement clause)",
       cmdclass = {'install': Install},
       packages = ['cjkcodecs'],
       ext_modules =
            [Extension("cjkcodecs._multibytecodec",
                       ["%s/multibytecodec.c" % SRCDIR], library_dirs=LIBDIRS)]
            + extensions
       )
