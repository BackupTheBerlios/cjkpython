#!/usr/bin/env python
#
# test_mapping_euc_jisx0213.py: Mapping test for EUC-JISX0213 codec
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
# $Id: test_mapping_euc_jisx0213.py,v 1.1 2003/09/24 17:47:03 perky Exp $
#

from test import test_support
import test_multibytecodec_support
import sys, codecs, os
import unittest

if not os.path.exists('EUC-JISX0213.TXT'):
    raise test_support.TestSkipped(
            'EUC-JISX0213.TXT not found, download from http://people.freebsd'
            '.org/~perky/i18n/EUC-JISX0213.TXT')

class TestEUCJISX0213Map(test_multibytecodec_support.TestBase_Mapping,
                   unittest.TestCase):
    encoding = 'cjkcodecs.euc_jisx0213'
    mapfilename = 'EUC-JISX0213.TXT'

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestEUCJISX0213Map))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 et
