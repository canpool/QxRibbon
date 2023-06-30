#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

QX_RIBBON_VERSION_STR = "0.1.0"


class Ribbon:
    """Microsoft Ribbon for PyQt"""
    @staticmethod
    def version() -> str:
        return QX_RIBBON_VERSION_STR
