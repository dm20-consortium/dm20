#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import glob

from context import Context
from testcase import TestCase

def result_output(result):
    if result:
        print("")
        print("==========")
        print("PASS")
        print("==========")
        sys.exit(0)

    else:
        print("")
        print("==========")
        print("FAIL")
        print("==========")
        sys.exit(1)

def main():

    if len(sys.argv) != 2:
        print("Usage:")
        print("    python3 runner.py scenario.yaml")
        sys.exit(1)

    target = sys.argv[1]

    if os.path.isdir(target):
        files = sorted(glob.glob(os.path.join(target, "*.yaml")))
        overall = True
        for f in files:
            tc = TestCase(Context())
            tc.load(f)

            if not tc.run():
                overall = False
        result_output(overall)

    else:
        testcase = TestCase(Context())
        testcase.load(target)
        result = testcase.run()
        result_output(result)

if __name__ == "__main__":
    main()