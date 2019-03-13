#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("starting test")
    child.expect_exact("true")
    child.expect_exact("false")
    child.expect_exact("true")

if __name__ == "__main__":
    sys.exit(run(testfunc))
