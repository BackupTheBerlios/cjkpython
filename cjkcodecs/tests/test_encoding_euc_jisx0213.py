#!/usr/bin/env python
#
# test_encoding_euc_jisx0213.py: Encoding test for the EUC-JISX0213 codec
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
# $Id: test_encoding_euc_jisx0213.py,v 1.1 2003/09/24 17:47:01 perky Exp $
#

from test import test_support
import test_multibytecodec_support
import unittest

class Test_EUC_JISX0213(test_multibytecodec_support.TestBase,
                        unittest.TestCase):
    encoding = 'cjkcodecs.euc_jisx0213'
    tstring = test_multibytecodec_support.load_teststring('euc_jisx0213')
    errortests = (
        # invalid bytes
        ("abc\x80\x80\xc1\xc4", "strict",  None),
        ("abc\xc8", "strict",  None),
        ("abc\x80\x80\xc1\xc4", "replace", u"abc\ufffd\u7956"),
        ("abc\x80\x80\xc1\xc4\xc8", "replace", u"abc\ufffd\u7956\ufffd"),
        ("abc\x80\x80\xc1\xc4", "ignore",  u"abc\u7956"),
        ("abc\x8f\x83\x83", "replace", u"abc\ufffd"),
        ("\xc1\x64", "strict", None),
        ("\xa1\xc0", "strict", u"\uff3c"),
    )
    xmlcharnametest = (
        u"\xab\u211c\xbb = \u2329\u1234\u232a",
        "\xa9\xa8&real;\xa9\xb2 = &lang;&#4660;&rang;"
    )

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(Test_EUC_JISX0213))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 et
