#!/usr/bin/env python
#
# test_codecmaps_tw.py
#   Codec mapping tests for ROC encodings
#
# $Id: test_codecmaps_tw.py,v 1.2 2004/01/17 12:47:19 perky Exp $

from test import test_support
import test_multibytecodec_support
import unittest

class TestBIG5Map(test_multibytecodec_support.TestBase_Mapping,
                  unittest.TestCase):
    encoding = 'cjkcodecs.big5'
    mapfilename = 'BIG5.TXT'
    mapfileurl = 'http://www.unicode.org/Public/MAPPINGS/OBSOLETE/' \
                 'EASTASIA/OTHER/BIG5.TXT'

class TestCP950Map(test_multibytecodec_support.TestBase_Mapping,
                   unittest.TestCase):
    encoding = 'cjkcodecs.cp950'
    mapfilename = 'CP950.TXT'
    mapfileurl = 'http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/' \
                 'WINDOWS/CP950.TXT'
    pass_enctest = [
        ('\xa2\xcc', u'\u5341'),
        ('\xa2\xce', u'\u5345'),
    ]

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestBIG5Map))
    suite.addTest(unittest.makeSuite(TestCP950Map))
    test_support.run_suite(suite)

test_multibytecodec_support.register_skip_expected(TestBIG5Map, TestCP950Map)
if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 sw=4 et
