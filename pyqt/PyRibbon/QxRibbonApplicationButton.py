#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QPushButton


class RibbonApplicationButton(QPushButton):

    def __init__(self, *args):
        """
        RibbonApplicationButton(parent=None)
        RibbonApplicationButton(text: str, parent=None)
        RibbonApplicationButton(icon: QIcon, text: str, parent=None)
        """
        super().__init__(*args)
        self.setFocusPolicy(Qt.NoFocus)
        self.setFlat(True)
        self.setMinimumWidth(40)
