#!/usr/bin/env python
#
# setup.py: Hangul Module Setup
#
# Copyright (C) 2002-2003 Hye-Shik Chang <perky@FreeBSD.org>.
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
# $Id: setup.py,v 1.1 2003/09/21 13:23:43 perky Exp $
#

import sys
from distutils.core import setup, Extension

LIBDIRS = []

if sys.platform == 'win32' and '--compiler=mingw32' in sys.argv:
    LIBDIRS.append('.') # libpython23.a and libpython23.def

setup (name = "hangul",
       version = "1.0",
       description = "Hangul Manipulation Module",
       author = "Hye-Shik Chang",
       author_email = "perky@FreeBSD.org",
       url = "http://cjkpython.berlios.de",
       license = "BSD Style (without advertisement clause)",
       ext_modules =
            [Extension("hangul", ["hangul.c"], library_dirs=LIBDIRS)]
       )

# ex: ts=8 sts=4 et
