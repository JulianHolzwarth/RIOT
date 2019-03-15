#!/usr/bin/env python3

# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("starting test")
    child.expect_exact("semaphore is created and available")
    child.expect_exact("First semaphore block, as expected true")
    child.expect_exact("Second semaphore block, as expected false", TI)
    child.expect_exact("Semaphore unblock, as expected true")

if __name__ == "__main__":
    sys.exit(run(testfunc))
