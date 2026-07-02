#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import yaml

from prepare import PrepareRunner
from service import ServiceRunner
from step import StepRunner
from expect import ExpectRunner
from cleanup import CleanupRunner


class TestCase:

    def __init__(self, context):

        self.ctx = context

        self.prepare = PrepareRunner(context)
        self.service = ServiceRunner(context)
        self.step = StepRunner(context)
        self.expect = ExpectRunner(context)
        self.cleanup = CleanupRunner(context)

    def load(self, yaml_file):

        self.ctx.yaml_file = yaml_file

        with open(yaml_file, encoding="utf-8") as f:
            self.ctx.config = yaml.safe_load(f)

    def print_header(self):

        cfg = self.ctx.config

        print("")
        print("===================================")
        print("Test ID    :", cfg.get("id", ""))
        print("Test Name  :", cfg.get("name", ""))

        if "description" in cfg:
            print("")
            print(cfg["description"])

        print("===================================")
        print("")

    def run(self):

        self.print_header()

        success = False

        try:

            self.prepare.run()

            self.service.start()

            self.step.run()

            success = self.expect.verify()

        except Exception as e:

            print("")
            print("Exception")
            print(e)

            success = False

        finally:

            self.cleanup.run()

        return success