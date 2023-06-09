#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

from typing import Union

from PyQt5.QtCore import QSize, QPoint
from PyQt5.QtGui import QPaintEvent, QPainter
from PyQt5.QtWidgets import QWidget, QCheckBox, QComboBox, QToolButton, \
    QLineEdit, QMenu, QAction, QSizePolicy, QWidgetAction


class RibbonControl(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)


class RibbonCheckBox(QCheckBox):
    def __init__(self, parent=None):
        super().__init__(parent)


class RibbonComboBox(QComboBox):
    def __init__(self, parent=None):
        super().__init__(parent)


class RibbonControlButton(QToolButton):
    def __init__(self, parent=None):
        super().__init__(parent)


class RibbonLineEdit(QLineEdit):
    def __init__(self, parent=None):
        super().__init__(parent)


class RibbonMenu(QMenu):
    def __init__(self, *args):
        """
        RibbonMenu(parent=None)
        RibbonMenu(title: str, parent=None)
        """
        parent = None
        arg_len = len(args)
        if arg_len > 0 and isinstance(args[0], str):
            title = args[0]
            if arg_len > 1 and isinstance(args[1], QWidget):
                parent = args[1]
            super().__init__(title, parent)
        else:
            if arg_len > 0 and isinstance(args[0], QWidget):
                parent = args[0]
            super().__init__(parent)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

    def addRibbonMenu(self, *args) -> Union[QAction, QMenu]:
        """
        addRibbonMenu(menu: RibbonMenu) -> QAction
        addRibbonMenu(title: str) -> RibbonMenu
        addRibbonMenu(icon: QIcon, title: str) -> RibbonMenu
        """
        arg_len = len(args)
        if arg_len == 1 and isinstance(args[0], RibbonMenu):
            return super().addMenu(args[0])
        elif arg_len == 1 and isinstance(args[0], str):
            menu = RibbonMenu(args[0], self)
            super().addAction(menu.menuAction())
            return menu
        else:
            assert arg_len == 2
            menu = RibbonMenu(args[1], self)
            menu.setIcon(args[0])
            super().addAction(menu.menuAction())
            return menu

    def addWidget(self, w: QWidget) -> QAction:
        action = QWidgetAction(self)
        action.setDefaultWidget(w)
        self.addAction(action)
        return action


class RibbonSeparator(QWidget):

    def __init__(self, *args):
        """
        RibbonMenu(parent=None)
        RibbonMenu(height: int, parent=None)
        """
        self.m_topMargin = 4
        self.m_bottomMargin = 4
        parent = None
        arg_len = len(args)
        if arg_len > 0 and isinstance(args[0], int):
            height = args[0]
            if arg_len > 1 and isinstance(args[1], QWidget):
                parent = args[1]
            super().__init__(parent)
            self.setFixedSize(6, height)
        else:
            if arg_len > 0 and isinstance(args[0], QWidget):
                parent = args[0]
            super().__init__(parent)
            self.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)
            self.setFixedWidth(6)

    def sizeHint(self) -> QSize:
        return QSize(6, self.height())

    def setTopBottomMargins(self, top: int, bottom: int):
        self.m_topMargin = top
        self.m_bottomMargin = bottom

    def paintEvent(self, event: QPaintEvent):
        painter = QPainter(self)
        painter.setPen(self.palette().window().color().darker(114))
        x1 = self.rect().center().x()
        painter.drawLine(QPoint(x1, self.rect().top() + self.m_topMargin),
                         QPoint(x1, self.rect().bottom() - self.m_bottomMargin))
