#!/usr/bin/env python
#
# test_codecmaps_cn.py
#   Codec mapping tests for PRC encodings
#
# $Id: test_codecmaps_cn.py,v 1.1 2003/12/19 02:39:09 perky Exp $

from test import test_support
import test_multibytecodec_support
import unittest

class TestGB2312Map(test_multibytecodec_support.TestBase_Mapping,
                   unittest.TestCase):
    encoding = 'cjkcodecs.gb2312'
    mapfilename = 'EUC-CN.TXT'
    mapfileurl = 'http://people.freebsd.org/~perky/i18n/EUC-CN.TXT'

class TestGBKMap(test_multibytecodec_support.TestBase_Mapping,
                   unittest.TestCase):
    encoding = 'cjkcodecs.gbk'
    mapfilename = 'CP936.TXT'
    mapfileurl = 'http://www.unicode.org/Public/MAPPINGS/VENDORS/' \
                 'MICSFT/WINDOWS/CP936.TXT'

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestGB2312Map))
    suite.addTest(unittest.makeSuite(TestGBKMap))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 sw=4 et
