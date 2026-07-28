#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess
import time

from step_result import StepResult
from string import Template


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

        command = Template(value["command"]).safe_substitute(self.ctx.params)

        print("Run Command")
        print(command)

        result = subprocess.run(
            command,
            shell=True,
            capture_output=True,
            text=True
        )

        #
        # 標準出力をそのまま表示
        #
        if result.stdout:
            print(result.stdout)

        #
        # 標準エラーも表示
        #
        if result.stderr:
            print(result.stderr)

        #
        # Step実行結果を保存
        #
        step_result = StepResult()

        step_result.command = command
        step_result.stdout = result.stdout
        step_result.stderr = result.stderr
        step_result.returncode = result.returncode

        #
        # 現在は最後に実行したrunのみ保存
        #
        self.ctx.add_step_result("last", step_result)

        #
        # エラーなら例外
        #
        if result.returncode != 0:

            raise RuntimeError(
                f"Command failed ({result.returncode})"
            )