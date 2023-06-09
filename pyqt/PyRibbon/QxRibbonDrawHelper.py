#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

from PyQt5.QtCore import Qt, QSize, QRect
from PyQt5.QtGui import QPixmap, QIcon, QPainter
from PyQt5.QtWidgets import QWidget, QStyleOption, QStyle


class RibbonDrawHelper:

    @staticmethod
    def iconToPixmap(icon: QIcon, widget: QWidget, opt: QStyleOption, icon_size: QSize) -> QPixmap:
        mode = QIcon.Normal
        if not (opt.state & QStyle.State_Enabled):
            mode = QIcon.Disabled
        elif (opt.state & QStyle.State_MouseOver) and (opt.state & QStyle.State_AutoRaise):
            mode = QIcon.Active

        state = QIcon.Off
        if (opt.state & QStyle.State_Selected) or (opt.state & QStyle.State_On):
            state = QIcon.On

        return icon.pixmap(widget.window().windowHandle(), icon_size, mode, state)

    @staticmethod
    def drawIcon(icon: QIcon, painter: QPainter, opt: QStyleOption, *args):
        """
        drawIcon(QIcon, QPainter, QStyleOption, x: int, y: int, width: int, height: int)
        drawIcon(QIcon, QPainter, QStyleOption, rect: QRect)
        """
        if len(args) < 1:
            return
        if isinstance(args[0], int):
            rect = QRect(args[0], args[1], args[2], args[3])
        else:
            rect = args[0]

        mode = QIcon.Normal
        if not (opt.state & QStyle.State_Enabled):
            mode = QIcon.Disabled
        elif (opt.state & QStyle.State_MouseOver) and (opt.state & QStyle.State_AutoRaise):
            mode = QIcon.Active

        state = QIcon.Off
        if (opt.state & QStyle.State_Selected) or (opt.state & QStyle.State_On):
            state = QIcon.On

        icon.paint(painter, rect, Qt.AlignCenter, mode, state)

    @staticmethod
    def iconActualSize(icon: QIcon, opt: QStyleOption, icon_size: QSize) -> QSize:
        mode = QIcon.Normal
        if not (opt.state & QStyle.State_Enabled):
            mode = QIcon.Disabled
        elif (opt.state & QStyle.State_MouseOver) and (opt.state & QStyle.State_AutoRaise):
            mode = QIcon.Active

        state = QIcon.Off
        if (opt.state & QStyle.State_Selected) or (opt.state & QStyle.State_On):
            state = QIcon.On

        return icon.actualSize(icon_size, mode, state)

    @staticmethod
    def drawText(text: str, painter: QPainter, opt: QStyleOption, align, *args):
        """
        drawText(str, QPainter, QStyleOption, Qt.Alignment, x: int, y: int, width: int, height: int)
        drawText(str, QPainter, QStyleOption, Qt.Alignment, rect: QRect)
        """
        if len(args) < 1:
            return
        if isinstance(args[0], int):
            rect = QRect(args[0], args[1], args[2], args[3])
        else:
            rect = args[0]
        painter.drawItemText(rect, align, opt.palette, opt.state & QStyle.State_Enabled, text)
