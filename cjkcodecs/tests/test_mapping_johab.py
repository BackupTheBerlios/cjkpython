#!/usr/bin/env python
#
# test_mapping_johab.py: Mapping test for JOHAB codec
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
# $Id: test_mapping_johab.py,v 1.1 2003/09/24 17:47:04 perky Exp $
#

from test import test_support
import test_multibytecodec_support
import sys, codecs, os
import unittest

if not os.path.exists('JOHAB.TXT'):
    raise test_support.TestSkipped(
            'JOHAB.TXT not found, download from http://www.unicode.'
            'org/Public/MAPPINGS/OBSOLETE/EASTASIA/KSC/JOHAB.TXT')

class TestJOHABMap(test_multibytecodec_support.TestBase_Mapping,
                   unittest.TestCase):
    encoding = 'cjkcodecs.johab'
    mapfilename = 'JOHAB.TXT'
    # KS X 1001 standard assigned 0x5c as WON SIGN.
    # but, in early 90s that is the only era used johab widely,
    # the most softwares implements it as REVERSE SOLIDUS.
    # So, we ignore the standard here.
    pass_enctest = [('\\', u'\u20a9')]
    pass_dectest = [('\\', u'\u20a9')]

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestJOHABMap))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 et
