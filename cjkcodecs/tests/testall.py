#!/usr/bin/env python
#
# testall.py: Run all unittests.
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
# $Id: testall.py,v 1.1 2003/09/24 17:47:05 perky Exp $
#

import sys
import os, unittest
from test import test_support

def findtests(testdir='.'):
    tests = []
    for name in os.listdir(testdir):
        if (name.startswith('test_') and name.endswith('.py')
                and not name.endswith('support.py')):
            tests.append(name[:-3])
    return tests

def loadtests(test):
    mod = __import__(test, globals(), locals(), [])
    return unittest.findTestCases(mod)

def main():
    suite = unittest.TestSuite()
    for test in findtests():
        try:
            suite.addTest(loadtests(test))
        except test_support.TestSkipped, msg:
            print "%s - skipped: %s" % (test, msg)
    test_support.run_suite(suite)

if __name__ == '__main__':
    main()

# ex: ts=8 sts=4 et
