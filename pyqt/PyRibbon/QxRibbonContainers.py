#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

from PyQt5.QtCore import Qt, QSize
from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel, QSizePolicy


class RibbonCtrlContainer(QWidget):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_widget = None
        self.m_icon = QIcon()
        self.m_iconSize = QSize(18, 18)

        self.m_layout = QHBoxLayout(self)
        self.m_layout.setSpacing(0);
        self.m_layout.setObjectName("layout");
        self.m_layout.setContentsMargins(0, 0, 0, 0);

        self.m_labelPixmap = QLabel(self)
        self.m_labelPixmap.setObjectName("labelPixmap")
        self.m_labelPixmap.setAlignment(Qt.AlignCenter)
        self.m_layout.addWidget(self.m_labelPixmap)

        self.m_labelText = QLabel(self)
        self.m_labelText.setObjectName("labelTest")
        self.m_layout.addWidget(self.m_labelText)

    def setIconVisible(self, b: bool):
        self.m_labelPixmap.setVisible(b)

    def setTitleVisible(self, b: bool):
        self.m_labelText.setVisible(b)

    def setIcon(self, i: QIcon):
        self.m_icon = i
        self.m_labelPixmap.setPixmap(i.pixmap(self.m_iconSize))

    def getIcon(self) -> QIcon:
        return self.m_icon

    def setText(self, t: str):
        self.m_labelText.setText(t)

    def getText(self) -> str:
        return self.m_labelText.text()

    def hasWidget(self) -> bool:
        return self.m_widget is not None

    def setWidget(self, w: QWidget):
        if self.m_widget:
            old_widget = self.m_widget
            i = self.m_layout.indexOf(old_widget)
            if i >= 0:
                self.m_layout.takeAt(i)
        self.m_widget = w
        if not w:
            return
        if w.parent() != self:
            w.setParent(self)
        self.m_layout.addWidget(w)
        sp = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        sp.setHorizontalStretch(0)
        sp.setVerticalStretch(0)
        w.setSizePolicy(sp)

    def getWidget(self) -> QWidget:
        return self.m_widget
