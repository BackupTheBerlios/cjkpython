#!/usr/bin/env python
#
# test_encoding_cp932.py: Encoding test for the CP932 codec
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
# $Id: test_encoding_cp932.py,v 1.1 2003/09/24 17:47:01 perky Exp $
#

from test import test_support
import test_multibytecodec_support
import unittest

class Test_CP932(test_multibytecodec_support.TestBase, unittest.TestCase):
    encoding = 'cjkcodecs.cp932'
    tstring = test_multibytecodec_support.load_teststring('shift_jis')
    errortests = (
        # invalid bytes
        ("abc\x81\x00\x81\x00\x82\x84", "strict",  None),
        ("abc\xf8", "strict",  None),
        ("abc\x81\x00\x82\x84", "replace", u"abc\ufffd\uff44"),
        ("abc\x81\x00\x82\x84\x88", "replace", u"abc\ufffd\uff44\ufffd"),
        ("abc\x81\x00\x82\x84", "ignore",  u"abc\uff44"),
        # sjis vs cp932
        ("\\\x7e", "replace", u"\\\x7e"),
        ("\x81\x5f\x81\x61\x81\x7c", "replace", u"\uff3c\u2225\uff0d"),
    )

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(Test_CP932))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 et
