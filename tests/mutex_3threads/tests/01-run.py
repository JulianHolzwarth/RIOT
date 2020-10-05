#!/usr/bin/env python3

#  Copyright (C) 2020 Freie Universität Berlin,
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>

import sys
from testrunner import run


def testfunc(child):
    child.expect("3rd")
    child.expect("2nd")
    child.expect("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))
