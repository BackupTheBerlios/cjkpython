#!/usr/bin/env python
#
# test_encoding_gb18030.py: Encoding test for the GB18030 codec
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
# $Id: test_encoding_gb18030.py,v 1.1 2003/09/24 17:47:02 perky Exp $
#

from test import test_support
import test_multibytecodec_support
import unittest

class Test_GB18030(test_multibytecodec_support.TestBase, unittest.TestCase):
    encoding = 'cjkcodecs.gb18030'
    tstring = test_multibytecodec_support.load_teststring('gb18030')
    errortests = (
        # invalid bytes
        ("abc\x80\x80\xc1\xc4", "strict",  None),
        ("abc\xc8", "strict",  None),
        ("abc\x80\x80\xc1\xc4", "replace", u"abc\ufffd\u804a"),
        ("abc\x80\x80\xc1\xc4\xc8", "replace", u"abc\ufffd\u804a\ufffd"),
        ("abc\x80\x80\xc1\xc4", "ignore",  u"abc\u804a"),
        ("abc\x84\x39\x84\x39\xc1\xc4", "replace", u"abc\ufffd\u804a"),
    )
    has_iso10646 = True

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(Test_GB18030))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 et