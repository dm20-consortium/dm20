#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pathlib import Path


class ExpectRunner:

    """
    YAML expect: を検証するクラス
    """

    def __init__(self, context):

        self.ctx = context

        #
        # Expect名と処理の対応
        #
        self.handlers = {
            "receiver_log": self.expect_receiver_log,
        }

    def verify(self):

        expect = self.ctx.config.get("expect", {})

        if len(expect) == 0:
            return True

        print("========== Verify ==========")

        success = True

        for key, value in expect.items():

            print()
            print(f"Verify [{key}]")

            if key not in self.handlers:
                raise RuntimeError(f"Unknown expect [{key}]")

            result = self.handlers[key](value)

            success &= result

        print()
        print("============================")

        return success

    #
    # receiver_log
    #
    def expect_receiver_log(self, value):

        logfile = Path("receiver.log")

        if not logfile.exists():

            print("receiver.log does not exist.")

            return False

        text = logfile.read_text(encoding="utf-8", errors="ignore")

        #
        # contains
        #
        contains = value.get("contains", [])

        ok = True

        for s in contains:

            if s in text:

                print(f"PASS : {s}")

            else:

                print(f"FAIL : {s}")

                ok = False

        return ok