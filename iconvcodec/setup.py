#!/usr/bin/env python
#
# setup.py: iconvcodec setup
# 
# Copyright (C) 2003 Hye-Shik Chang. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $Id: setup.py,v 1.1 2003/11/27 09:04:45 perky Exp $
#

import sys
from distutils.core import setup, Extension
from distutils.command.install import install

include_dirs = []
library_dirs = []
libraries = []
use_libiconv = 1

for arg in sys.argv[1:]: # don't use getopt to ignore arguments for distutils
    args = arg.split('=', 1)
    if args[0] == '--with-libc' or args[0] == '--without-libiconv':
        use_libiconv = 0
    elif args[0] == '--with-libiconv':
        use_libiconv = 1
        if len(args) > 1 and args[1].lower() != 'yes':
            include_dirs = [args[1] + '/include']
            library_dirs = [args[1] + '/lib']
            libraries = ['iconv']
    elif args[0] == '--help':
        print """\
Iconvcodec options:
  --with-libc               use iconv incorporated in libc
  --with-libiconv[=path]    use separated libiconv (default)
"""
        continue
    else:
        continue

    sys.argv.remove(arg)

if use_libiconv and not libraries:
    include_dirs = ['/usr/local/include', '/sw/include', '/usr/pkg/include']
    library_dirs = ['/usr/local/lib', '/sw/lib', '/usr/pkg/lib']
    libraries = ['iconv']

class Install(install):
    def initialize_options(self):
        install.initialize_options(self)
        self.extra_path = ("iconv_codec", "import iconv_codec")
    def finalize_options(self):
        org_install_lib = self.install_lib
        install.finalize_options(self)
        self.install_libbase = self.install_lib = \
            org_install_lib or self.install_purelib

setup (name = "iconvcodec",
       version = "1.1.1",
       author = "Hye-Shik Chang",
       author_email = "perky@FreeBSD.org",
       cmdclass = {'install': Install},
       url = 'http://sourceforge.net/projects/koco/',
       py_modules = ['iconv_codec'],
       ext_modules = [
           Extension("_iconv_codec",
                     ["_iconv_codec.c"],
                     include_dirs = include_dirs,
                     library_dirs = library_dirs,
                     libraries = libraries),
       ])

# ex: ts=8 sts=4 et
