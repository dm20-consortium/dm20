#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pathlib import Path


class Context:
    """
    テスト実行中の共有情報
    """

    def __init__(self):

        # YAML全体
        self.config = {}

        # YAMLファイル名
        self.yaml_file = ""

        # カレントディレクトリ
        self.workdir = Path.cwd()

        # 起動中プロセス
        # {"dm2is": subprocess.Popen}
        self.processes = {}

        # ログファイル
        # {"dm2is": open()}
        self.logfiles = {}

    def add_process(self, name, process):
        self.processes[name] = process

    def get_process(self, name):
        return self.processes.get(name)

    def add_logfile(self, name, logfile):
        self.logfiles[name] = logfile

    def get_logfile(self, name):
        return self.logfiles.get(name)