#!/usr/bin/env python
#
# testall.py
#   Run all unittests.
#
# $Id: testall.py,v 1.3 2004/06/19 06:09:55 perky Exp $

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
