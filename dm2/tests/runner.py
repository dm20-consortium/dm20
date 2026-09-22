#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import glob

from context import Context
from testcase import TestCase

def result_output(result, failed_cases):
    print("")
    print("==========")
    if result:
        print("PASS")
        print("==========")
        sys.exit(0)

    else:
        print(f"FAIL : {', '.join(failed_cases)}")
        print("==========")
        sys.exit(1)

def main():

    if len(sys.argv) != 2:
        print("Usage:")
        print("    python3 runner.py scenario.yaml")
        sys.exit(1)

    target = sys.argv[1]

    failed_cases = []
    if os.path.isdir(target):
        files = sorted(glob.glob(os.path.join(target, "*.yaml")))
        overall = True
        for i, f in enumerate(files, start=1):
            tc = TestCase(Context())
            tc.load(f)

            if not tc.run():
                overall = False
                failed_cases.append(tc.ctx.config["id"])
        result_output(overall, failed_cases)

    else:
        testcase = TestCase(Context())
        testcase.load(target)
        result = testcase.run()
        result_output(result, failed_cases)

if __name__ == "__main__":
    main()