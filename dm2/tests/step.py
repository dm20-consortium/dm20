#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess
import time


class StepRunner:

    """
    YAML steps: を実行するクラス
    """

    def __init__(self, context):

        self.ctx = context

        #
        # Step名と処理の対応表
        #
        self.handlers = {
            "wait": self.step_wait,
            "run": self.step_run,
        }

    def run(self):

        steps = self.ctx.config.get("steps", [])

        if len(steps) == 0:
            return

        print("========== Execute Steps ==========")

        for index, step in enumerate(steps, start=1):

            print()
            print(f"Step {index}")

            self.execute(step)

        print()
        print("===================================")

    def execute(self, step):

        #
        # Stepは必ず1要素だけ持つ
        #
        # - wait: 2
        #
        # または
        #
        # - run:
        #
        step_name = list(step.keys())[0]

        if step_name not in self.handlers:

            raise RuntimeError(
                f"Unknown step [{step_name}]"
            )

        self.handlers[step_name](step[step_name])

    #
    # wait
    #
    def step_wait(self, value):

        print(f"Wait {value} sec")

        time.sleep(value)

    #
    # run
    #
    def step_run(self, value):

        command = value["command"]

        print("Run Command")
        print(command)

        result = subprocess.run(
            command,
            shell=True
        )

        if result.returncode != 0:

            raise RuntimeError(
                f"Command failed ({result.returncode})"
            )