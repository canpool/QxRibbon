#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT


from PyQt5.QtWidgets import QAction, QWidget, QMenu, QToolButton

from .QxRibbonContainers import RibbonCtrlContainer
from .QxRibbonButtonGroup import RibbonButtonGroup


class RibbonQuickAccessBar(RibbonCtrlContainer):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_buttonGroup = RibbonButtonGroup(self)
        self.setWidget(self.m_buttonGroup)

    def addSeparator(self):
        self.m_buttonGroup.addSeparator()

    def addAction(self, action: QAction):
        self.m_buttonGroup.addAction(action)

    def addWidget(self, w: QWidget):
        self.m_buttonGroup.addWidget(w)

    def addMenu(self, m: QMenu, pop_mode=QToolButton.InstantPopup):
        self.m_buttonGroup.addMenu(m, pop_mode)
