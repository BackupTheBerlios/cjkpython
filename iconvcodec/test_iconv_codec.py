#
# test_iconv_codec.py: Testunit for iconv_codec
#
# Copyright (C) 2003 Hye-Shik Chang. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $Id: test_iconv_codec.py,v 1.1 2003/11/27 09:04:45 perky Exp $

from StringIO import StringIO
import sys, codecs
import unittest
import iconv_codec
from test import test_support

teststring_euc_kr = (
"\xa1\xdd\x20\xc6\xc4\xc0\xcc\xbd\xe3\x28\x50\x79\x74\x68\x6f\x6e"
"\x29\xc0\xba\x20\xb9\xe8\xbf\xec\xb1\xe2\x20\xbd\xb1\xb0\xed\x2c"
"\x20\xb0\xad\xb7\xc2\xc7\xd1\x20\xc7\xc1\xb7\xce\xb1\xd7\xb7\xa1"
"\xb9\xd6\x20\xbe\xf0\xbe\xee\xc0\xd4\xb4\xcf\xb4\xd9\x2e\x20\xc6"
"\xc4\xc0\xcc\xbd\xe3\xc0\xba\x0a\xc8\xbf\xc0\xb2\xc0\xfb\xc0\xce"
"\x20\xb0\xed\xbc\xf6\xc1\xd8\x20\xb5\xa5\xc0\xcc\xc5\xcd\x20\xb1"
"\xb8\xc1\xb6\xbf\xcd\x20\xb0\xa3\xb4\xdc\xc7\xcf\xc1\xf6\xb8\xb8"
"\x20\xc8\xbf\xc0\xb2\xc0\xfb\xc0\xce\x20\xb0\xb4\xc3\xbc\xc1\xf6"
"\xc7\xe2\xc7\xc1\xb7\xce\xb1\xd7\xb7\xa1\xb9\xd6\xc0\xbb\x0a\xc1"
"\xf6\xbf\xf8\xc7\xd5\xb4\xcf\xb4\xd9\x2e\x20\xc6\xc4\xc0\xcc\xbd"
"\xe3\xc0\xc7\x20\xbf\xec\xbe\xc6\x28\xe9\xd0\xe4\xba\x29\xc7\xd1"
"\x20\xb9\xae\xb9\xfd\xb0\xfa\x20\xb5\xbf\xc0\xfb\x20\xc5\xb8\xc0"
"\xcc\xc7\xce\x2c\x20\xb1\xd7\xb8\xae\xb0\xed\x20\xc0\xce\xc5\xcd"
"\xc7\xc1\xb8\xae\xc6\xc3\x0a\xc8\xaf\xb0\xe6\xc0\xba\x20\xc6\xc4"
"\xc0\xcc\xbd\xe3\xc0\xbb\x20\xbd\xba\xc5\xa9\xb8\xb3\xc6\xc3\xb0"
"\xfa\x20\xbf\xa9\xb7\xc1\x20\xba\xd0\xbe\xdf\xbf\xa1\xbc\xad\xbf"
"\xcd\x20\xb4\xeb\xba\xce\xba\xd0\xc0\xc7\x20\xc7\xc3\xb7\xa7\xc6"
"\xfb\xbf\xa1\xbc\xad\xc0\xc7\x20\xba\xfc\xb8\xa5\x0a\xbe\xd6\xc7"
"\xc3\xb8\xae\xc4\xc9\xc0\xcc\xbc\xc7\x20\xb0\xb3\xb9\xdf\xc0\xbb"
"\x20\xc7\xd2\x20\xbc\xf6\x20\xc0\xd6\xb4\xc2\x20\xc0\xcc\xbb\xf3"
"\xc0\xfb\xc0\xce\x20\xbe\xf0\xbe\xee\xb7\xce\x20\xb8\xb8\xb5\xe9"
"\xbe\xee\xc1\xdd\xb4\xcf\xb4\xd9\x2e\x0a",
"\xe2\x97\x8e\x20\xed\x8c\x8c\xec\x9d\xb4\xec\x8d\xac\x28\x50\x79"
"\x74\x68\x6f\x6e\x29\xec\x9d\x80\x20\xeb\xb0\xb0\xec\x9a\xb0\xea"
"\xb8\xb0\x20\xec\x89\xbd\xea\xb3\xa0\x2c\x20\xea\xb0\x95\xeb\xa0"
"\xa5\xed\x95\x9c\x20\xed\x94\x84\xeb\xa1\x9c\xea\xb7\xb8\xeb\x9e"
"\x98\xeb\xb0\x8d\x20\xec\x96\xb8\xec\x96\xb4\xec\x9e\x85\xeb\x8b"
"\x88\xeb\x8b\xa4\x2e\x20\xed\x8c\x8c\xec\x9d\xb4\xec\x8d\xac\xec"
"\x9d\x80\x0a\xed\x9a\xa8\xec\x9c\xa8\xec\xa0\x81\xec\x9d\xb8\x20"
"\xea\xb3\xa0\xec\x88\x98\xec\xa4\x80\x20\xeb\x8d\xb0\xec\x9d\xb4"
"\xed\x84\xb0\x20\xea\xb5\xac\xec\xa1\xb0\xec\x99\x80\x20\xea\xb0"
"\x84\xeb\x8b\xa8\xed\x95\x98\xec\xa7\x80\xeb\xa7\x8c\x20\xed\x9a"
"\xa8\xec\x9c\xa8\xec\xa0\x81\xec\x9d\xb8\x20\xea\xb0\x9d\xec\xb2"
"\xb4\xec\xa7\x80\xed\x96\xa5\xed\x94\x84\xeb\xa1\x9c\xea\xb7\xb8"
"\xeb\x9e\x98\xeb\xb0\x8d\xec\x9d\x84\x0a\xec\xa7\x80\xec\x9b\x90"
"\xed\x95\xa9\xeb\x8b\x88\xeb\x8b\xa4\x2e\x20\xed\x8c\x8c\xec\x9d"
"\xb4\xec\x8d\xac\xec\x9d\x98\x20\xec\x9a\xb0\xec\x95\x84\x28\xe5"
"\x84\xaa\xe9\x9b\x85\x29\xed\x95\x9c\x20\xeb\xac\xb8\xeb\xb2\x95"
"\xea\xb3\xbc\x20\xeb\x8f\x99\xec\xa0\x81\x20\xed\x83\x80\xec\x9d"
"\xb4\xed\x95\x91\x2c\x20\xea\xb7\xb8\xeb\xa6\xac\xea\xb3\xa0\x20"
"\xec\x9d\xb8\xed\x84\xb0\xed\x94\x84\xeb\xa6\xac\xed\x8c\x85\x0a"
"\xed\x99\x98\xea\xb2\xbd\xec\x9d\x80\x20\xed\x8c\x8c\xec\x9d\xb4"
"\xec\x8d\xac\xec\x9d\x84\x20\xec\x8a\xa4\xed\x81\xac\xeb\xa6\xbd"
"\xed\x8c\x85\xea\xb3\xbc\x20\xec\x97\xac\xeb\xa0\xa4\x20\xeb\xb6"
"\x84\xec\x95\xbc\xec\x97\x90\xec\x84\x9c\xec\x99\x80\x20\xeb\x8c"
"\x80\xeb\xb6\x80\xeb\xb6\x84\xec\x9d\x98\x20\xed\x94\x8c\xeb\x9e"
"\xab\xed\x8f\xbc\xec\x97\x90\xec\x84\x9c\xec\x9d\x98\x20\xeb\xb9"
"\xa0\xeb\xa5\xb8\x0a\xec\x95\xa0\xed\x94\x8c\xeb\xa6\xac\xec\xbc"
"\x80\xec\x9d\xb4\xec\x85\x98\x20\xea\xb0\x9c\xeb\xb0\x9c\xec\x9d"
"\x84\x20\xed\x95\xa0\x20\xec\x88\x98\x20\xec\x9e\x88\xeb\x8a\x94"
"\x20\xec\x9d\xb4\xec\x83\x81\xec\xa0\x81\xec\x9d\xb8\x20\xec\x96"
"\xb8\xec\x96\xb4\xeb\xa1\x9c\x20\xeb\xa7\x8c\xeb\x93\xa4\xec\x96"
"\xb4\xec\xa4\x8d\xeb\x8b\x88\xeb\x8b\xa4\x2e\x0a")

class TestBase:
    encoding        = ''   # codec name
    codec           = None # codec tuple (with 4 elements)
    tstring         = ''   # string to test StreamReader

    errortests      = None # must set. error test tuple
    roundtriptest   = 1    # set if roundtrip is possible with unicode
    has_iso10646    = 0    # set if this encoding contains whole iso10646 map

    def setUp(self):
        if self.codec is None:
            self.codec = codecs.lookup(self.encoding)
        self.encode, self.decode, self.reader, self.writer = self.codec

    def test_chunkcoding(self):
        for native, utf8 in zip(*[StringIO(f).readlines()
                                  for f in self.tstring]):
            u = self.decode(native)[0]
            self.assertEqual(u, utf8.decode('utf-8'))
            if self.roundtriptest:
                self.assertEqual(native, self.encode(u)[0])

    def test_errorhandle(self):
        for source, scheme, expected in self.errortests:
            if type(source) == type(''):
                func = self.decode
            else:
                func = self.encode
            if expected:
                result = func(source, scheme)[0]
                self.assertEqual(result, expected)
            else:
                self.assertRaises(UnicodeError, func, source, scheme)

    if sys.hexversion >= 0x02030000:
        def test_xmlcharrefreplace(self):
            if self.has_iso10646:
                return

            s = u"\u0b13\u0b23\u0b60 nd eggs"
            self.assertEqual(
                self.encode(s, "xmlcharrefreplace")[0],
                "&#2835;&#2851;&#2912; nd eggs"
            )

        def test_customreplace(self):
            if self.has_iso10646:
                return

            import htmlentitydefs

            names = {}
            for (key, value) in htmlentitydefs.entitydefs.items():
                if len(value)==1:
                    names[value.decode('latin-1')] = self.decode(key)[0]
                else:
                    names[unichr(int(value[2:-1]))] = self.decode(key)[0]

            def xmlcharnamereplace(exc):
                if not isinstance(exc, UnicodeEncodeError):
                    raise TypeError("don't know how to handle %r" % exc)
                l = []
                for c in exc.object[exc.start:exc.end]:
                    try:
                        l.append(u"&%s;" % names[c])
                    except KeyError:
                        l.append(u"&#%d;" % ord(c))
                return (u"".join(l), exc.end)

            codecs.register_error(
                "test.xmlcharnamereplace", xmlcharnamereplace)

            sin = u"\xab\u211c\xbb = \u2329\u1234\u232a"
            sout = "&laquo;&real;&raquo; = &lang;&#4660;&rang;"
            self.assertEqual(self.encode(sin,
                                        "test.xmlcharnamereplace")[0], sout)

    def test_streamreader(self):
        UTF8Writer = codecs.lookup('utf-8')[3]
        for name in ["read", "readline", "readlines"]:
            for sizehint in [None, -1] + range(1, 33) + \
                            [64, 128, 256, 512, 1024]:
                istream = self.reader(StringIO(self.tstring[0]))
                ostream = UTF8Writer(StringIO())
                func = getattr(istream, name)
                while 1:
                    data = func(sizehint)
                    if not data:
                        break
                    if name == "readlines":
                        ostream.writelines(data)
                    else:
                        ostream.write(data)

                self.assertEqual(ostream.getvalue(), self.tstring[1])

    def test_streamwriter(self):
        # We can't test with the real utf-8 StreamReader here.
        # The standard SR.readline{,s} are mostly broken for multibyte seqs.
        #UTF8Reader = codecs.lookup('utf-8')[2]
        UTF8Reader = iconv_codec.lookup('utf-8')[2]
        for name in ["read", "readline", "readlines"]:
            for sizehint in [None, -1] + range(1, 33) + \
                            [64, 128, 256, 512, 1024]:
                istream = UTF8Reader(StringIO(self.tstring[1]))
                ostream = self.writer(StringIO())
                func = getattr(istream, name)
                while 1:
                    data = func(sizehint)
                    if not data:
                        break
                    if name == "readlines":
                        ostream.writelines(data)
                    else:
                        ostream.write(data)

                self.assertEqual(ostream.getvalue(), self.tstring[0])

class Test_EUC_KR(TestBase, unittest.TestCase):
    encoding = 'euc-kr'
    codec = iconv_codec.lookup('euc-kr')
    tstring = teststring_euc_kr
    if sys.platform.startswith('sunos'):
        # SUN iconv encodes uncodable unicodes into '?'
        has_iso10646 = True
    errortests = (
        # invalid bytes
        ("abc\xa0\xa0\xc1\xc4", "strict",  None),
        ("abc\xc8", "strict",  None),
        ("abc\xa0\xc1\xc4", "replace", u"abc\ufffd\uc894"),
        ("abc\xa0\xa0\xc1\xc4\xc8", "replace", u"abc\ufffd\ufffd\uc894\ufffd"),
        ("abc\xa0\xa0\xc1\xc4", "ignore",  u"abc\uc894"),
        ("\xc1\x64", "strict", None),
    )

def test_main():
    if not iconv_codec.lookup('euc-kr'):
        raise test_support.TestSkipped(
                "your iconv doesn't have a support for euc-kr encoding")
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(Test_EUC_KR))
    test_support.run_suite(suite)

if __name__ == "__main__":
    test_main()

# ex: ts=8 sts=4 et
