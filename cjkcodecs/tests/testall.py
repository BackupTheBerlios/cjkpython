#!/usr/bin/env python
#
# testall.py
#   Run all unittests.
#
# $Id: testall.py,v 1.2 2003/12/19 03:01:29 perky Exp $

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

# ex: ts=8 sts=4 sw=4 et
