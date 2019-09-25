#!/usr/bin/env python3

import os
import sys
import time
import pexpect
from testrunner import run


def testfunc(child):
    # Try to wait for the shell
    for _ in range(0, 10):
        child.sendline("help")
        if child.expect_exact(["Command", pexpect.TIMEOUT], timeout=1) == 0:
            break
    child.expect('> ')

    child.sendline('create')
    child.expect('> ')

    # Add delay to be sure 'idle' ran or anything.
    time.sleep(1)
    child.sendline('ps')
    child.expect('> ')
    with open('ps_output.%s.before' % os.environ['BOARD'], 'w+') as ps_output:
        ps_output.write(child.before)

    child.sendline('kill 1')
    child.expect('> ')

    time.sleep(1)
    child.sendline('ps')
    child.expect('> ')
    with open('ps_output.%s.after' % os.environ['BOARD'], 'w+') as ps_output:
        ps_output.write(child.before)


if __name__ == "__main__":
    sys.exit(run(testfunc))
