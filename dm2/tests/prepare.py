#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess


class PrepareRunner:

    """
    YAML prepare: を実行するクラス
    """

    def __init__(self, context):

        self.ctx = context

    def run(self):

        prepare = self.ctx.config.get("prepare")

        if prepare is None:
            return

        print("========== Prepare ==========")

        #
        # Database
        #
        database = prepare.get("database")

        if database:

            self.prepare_database(database)

        print("=============================")

    def prepare_database(self, database):

        script = database.get("init_script")

        if script is None:
            return

        print("Initialize database")
        print(script)

        result = subprocess.run(
            f"sudo -u postgres bash {script}",
            shell=True
        )

        if result.returncode != 0:

            raise RuntimeError(
                "Database initialization failed."
            )