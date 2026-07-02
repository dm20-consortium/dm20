#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from service import ServiceRunner


class CleanupRunner:

    def __init__(self, context):

        self.service = ServiceRunner(context)

    def run(self):

        self.service.stop()