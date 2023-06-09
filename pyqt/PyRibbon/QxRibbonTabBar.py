#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

from PyQt5.QtCore import QMargins
from PyQt5.QtWidgets import QTabBar


class RibbonTabBar(QTabBar):

    def __init__(self, parent):
        super().__init__(parent)
        self.m_tabMargin: QMargins = QMargins(6, 0, 0, 0)
        self.setExpanding(False)

    def tabMargin(self) -> QMargins:
        return self.m_tabMargin

    def setTabMargin(self, margin: QMargins):
        self.m_tabMargin = margin
