#!/usr/bin/env python
#
# test_multibytecodec.py: Unit test for multibytecodec itself
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
# $Id: test_multibytecodec.py,v 1.1 2003/09/24 17:47:05 perky Exp $
#

from test import test_support
import test_multibytecodec_support
import unittest, StringIO, codecs

class Test_StreamWriter(unittest.TestCase):
    if len(u'\U00012345') == 2: # UCS2
        def test_gb18030(self):
            s= StringIO.StringIO()
            c = codecs.lookup('cjkcodecs.gb18030')[3](s)
            c.write(u'123')
            self.assertEqual(s.getvalue(), '123')
            c.write(u'\U00012345')
            self.assertEqual(s.getvalue(), '123\x907\x959')
            c.write(u'\U00012345'[0])
            self.assertEqual(s.getvalue(), '123\x907\x959')
            c.write(u'\U00012345'[1] + u'\U00012345' + u'\uac00\u00ac')
            self.assertEqual(s.getvalue(),
                    '123\x907\x959\x907\x959\x907\x959\x827\xcf5\x810\x851')
            c.write(u'\U00012345'[0])
            self.assertEqual(s.getvalue(),
                    '123\x907\x959\x907\x959\x907\x959\x827\xcf5\x810\x851')
            self.assertRaises(UnicodeError, c.reset)
            self.assertEqual(s.getvalue(),
                    '123\x907\x959\x907\x959\x907\x959\x827\xcf5\x810\x851')

        def test_utf_8(self):
            s= StringIO.StringIO()
            c = codecs.lookup('cjkcodecs.utf-8')[3](s)
            c.write(u'123')
            self.assertEqual(s.getvalue(), '123')
            c.write(u'\U00012345')
            self.assertEqual(s.getvalue(), '123\xf0\x92\x8d\x85')
            c.write(u'\U00012345'[0])
            self.assertEqual(s.getvalue(), '123\xf0\x92\x8d\x85')
            c.write(u'\U00012345'[1] + u'\U00012345' + u'\uac00\u00ac')
            self.assertEqual(s.getvalue(),
                '123\xf0\x92\x8d\x85\xf0\x92\x8d\x85\xf0\x92\x8d\x85'
                '\xea\xb0\x80\xc2\xac')
            c.write(u'\U00012345'[0])
            self.assertEqual(s.getvalue(),
                '123\xf0\x92\x8d\x85\xf0\x92\x8d\x85\xf0\x92\x8d\x85'
                '\xea\xb0\x80\xc2\xac')
            c.reset()
            self.assertEqual(s.getvalue(),
                '123\xf0\x92\x8d\x85\xf0\x92\x8d\x85\xf0\x92\x8d\x85'
                '\xea\xb0\x80\xc2\xac\xed\xa0\x88')
            c.write(u'\U00012345'[1])
            self.assertEqual(s.getvalue(),
                '123\xf0\x92\x8d\x85\xf0\x92\x8d\x85\xf0\x92\x8d\x85'
                '\xea\xb0\x80\xc2\xac\xed\xa0\x88\xed\xbd\x85')

    else: # UCS4
        pass

def test_main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(Test_StreamWriter))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 et
