#!/usr/bin/env python
#
# test_encoding_shift_jis.py: Encoding test for the Shift-JIS codec
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
# $Id: test_encoding_shift_jis.py,v 1.3 2003/11/27 18:55:34 perky Exp $
#

from test import test_support
import test_multibytecodec_support
import unittest

commonenctests = (
    ("abc\x80\x80\x82\x84", "strict",  None),
    ("abc\xf8", "strict",  None),
    ("abc\x80\x80\x82\x84", "replace", u"abc\ufffd\uff44"),
    ("abc\x80\x80\x82\x84\x88", "replace", u"abc\ufffd\uff44\ufffd"),
    ("abc\x80\x80\x82\x84def", "ignore",  u"abc\uff44def"),
)

class Test_SJIS_COMPAT(test_multibytecodec_support.TestBase, unittest.TestCase):
    encoding = 'cjkcodecs.shift_jis'
    tstring = test_multibytecodec_support.load_teststring('shift_jis')
    codectests = commonenctests + (
        ("\\\x7e", "strict", u"\\\x7e"),
        ("\x81\x5f\x81\x61\x81\x7c", "strict", u"\uff3c\u2016\u2212"),
    )

class Test_SJIS_STRICT(test_multibytecodec_support.TestBase, unittest.TestCase):
    encoding = 'cjkcodecs.shift_jis_strict'
    tstring = test_multibytecodec_support.load_teststring('shift_jis')
    codectests = commonenctests + (
        ("\\\x7e", "replace", u"\xa5\u203e"),
        ("\x81\x5f\x81\x61\x81\x7c", "replace", u"\x5c\u2016\u2212"),
    )

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(Test_SJIS_COMPAT))
    suite.addTest(unittest.makeSuite(Test_SJIS_STRICT))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 et
