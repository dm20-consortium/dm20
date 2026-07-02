#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess
import time
import shlex


class ServiceRunner:

    """
    YAML services: を管理するクラス
    """

    def __init__(self, context):

        self.ctx = context

    def start(self):

        services = self.ctx.config.get("services", [])

        if len(services) == 0:
            return

        print("========== Start Services ==========")

        for service in services:

            self.start_service(service)

        print("====================================")
        print()

    def start_service(self, service):

        name = service["name"]
        command = service["command"]

        logfile = open(f"{name}.log", "w")

        print(f"Start [{name}]")
        print(f"Command : {command}")

        process = None
        if name == "dm2is":
            process = subprocess.Popen(
                shlex.split(command),
                stdout=logfile,
                stderr=subprocess.STDOUT
            )
        else:
            process = subprocess.Popen(
                command,
                shell=True,
                stdout=logfile,
                stderr=subprocess.STDOUT
            )

        self.ctx.add_process(name, process)
        self.ctx.add_logfile(name, logfile)

        # 起動直後に少し待つ
        time.sleep(1)

        if process.poll() is not None:

            raise RuntimeError(
                f"{name} terminated immediately."
            )

    def stop(self):

        print()
        print("========== Stop Services ==========")

        #
        # YAML cleanup.stop があればそれを優先
        #
        cleanup = self.ctx.config.get("cleanup", {})

        targets = cleanup.get(
            "stop",
            list(self.ctx.processes.keys())
        )

        for name in targets:

            self.stop_service(name)

        print("===================================")

    def stop_service(self, name):

        process = self.ctx.get_process(name)

        if process is None:
            return

        print(f"Stop [{name}]")

        #
        # terminate
        #
        process.terminate()

        try:

            process.wait(timeout=5)

        except subprocess.TimeoutExpired:

            print(f"{name} does not terminate. Kill.")

            process.kill()

            process.wait()

        logfile = self.ctx.get_logfile(name)

        if logfile:
            logfile.close()