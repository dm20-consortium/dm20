#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

from context import Context
from testcase import TestCase


def main():

    if len(sys.argv) != 2:
        print("Usage:")
        print("    python3 runner.py scenario.yaml")
        sys.exit(1)

    yaml_file = sys.argv[1]

    ctx = Context()

    testcase = TestCase(ctx)

    testcase.load(yaml_file)

    result = testcase.run()

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


if __name__ == "__main__":
    main()