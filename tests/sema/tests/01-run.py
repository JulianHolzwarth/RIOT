#!/usr/bin/env python3

#  Copyright (C) 2019 Freie Universität Berlin,
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>

def testfunc(child):
    child.expect("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))
