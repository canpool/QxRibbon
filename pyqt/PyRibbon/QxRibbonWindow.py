#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

from PyQt5.QtCore import QFile, QIODevice
from PyQt5.QtCore import QObject, QEvent
from PyQt5.QtWidgets import QMainWindow, QApplication, QWidget, QMenuBar

from .FramelessHelper import FramelessHelper
from .QxWindowButtonGroup import WindowButtonGroup
from .QxRibbonBar import RibbonBar
from . import QxRibbonRes_rc


class RibbonWindow(QMainWindow):

    def __init__(self, parent=None, use_ribbon=True):
        super().__init__(parent)
        self.m_useRibbon = use_ribbon
        self.m_theme = RibbonWindow.Office2013Theme
        self.m_ribbonBar: RibbonBar = None
        self.m_windowButtonGroup: WindowButtonGroup = None
        self.m_framelessHelper: FramelessHelper = None

        if self.m_useRibbon:
            self.setRibbonTheme(self.ribbonTheme())
            self.setMenuWidget(RibbonBar(self))

    def ribbonBar(self) -> RibbonBar:
        return self.m_ribbonBar

    def setRibbonTheme(self, theme):
        if theme == RibbonWindow.NormalTheme:
            self.loadTheme(':/theme/res/default.qss')
        elif theme == RibbonWindow.Office2013Theme:
            self.loadTheme(':/theme/res/office2013.qss')
        else:
            self.loadTheme(':/theme/res/default.qss')
        self.m_theme = theme

    def ribbonTheme(self) -> int:
        return self.m_theme

    def isUseRibbon(self) -> bool:
        return self.m_useRibbon

    def updateWindowFlag(self, flags):
        if self.isUseRibbon():
            self.m_windowButtonGroup.updateWindowFlag(flags)
        self.repaint()

    def windowButtonFlags(self) -> int:
        if self.isUseRibbon():
            return self.m_windowButtonGroup.windowButtonFlags()
        return self.windowFlags()

    def _setMenuWidget(self, bar: QWidget):
        if isinstance(bar, RibbonBar):
            self.m_ribbonBar: RibbonBar = bar
            self.m_ribbonBar.installEventFilter(self)
            if self.m_framelessHelper is None:
                self.m_framelessHelper = FramelessHelper(self)
            self.m_framelessHelper.setTitleHeight(self.m_ribbonBar.titleBarHeight())
            if self.m_windowButtonGroup is None:
                self.m_windowButtonGroup = WindowButtonGroup(self)
            s = self.m_windowButtonGroup.sizeHint()
            s.setHeight(self.m_ribbonBar.titleBarHeight())
            self.m_windowButtonGroup.setFixedSize(s)
            self.m_windowButtonGroup.setWindowStates(self.windowState())
            self.m_useRibbon = True
            self.m_windowButtonGroup.show()
        else:
            self.m_ribbonBar = None
            self.m_useRibbon = False
            self.m_framelessHelper = None
            if self.m_windowButtonGroup:
                self.m_windowButtonGroup.hide()

    def setMenuWidget(self, bar: QWidget):
        super().setMenuWidget(bar)
        self._setMenuWidget(bar)

    def setMenuBar(self, bar: QMenuBar):
        super().setMenuBar(bar)
        self._setMenuWidget(bar)

    def resizeEvent(self, e):
        if self.m_ribbonBar:
            if self.m_ribbonBar.size().width() != self.size().width():
                self.m_ribbonBar.setFixedWidth(self.size().width())
            if self.m_windowButtonGroup:
                self.m_ribbonBar.setWindowButtonsSize(self.m_windowButtonGroup.size())
        super().resizeEvent(e)

    def eventFilter(self, obj: QObject, e: QEvent) -> bool:
        """
        过滤是为了把 ribbonBar 上的动作传递到 mainwindow，再传递到 frameless
        由于 ribbonbar 会遮挡 frameless 的区域，导致 frameless 无法捕获这些消息
        """
        if obj == self.m_ribbonBar:
            e_list = (
                QEvent.MouseButtonPress,
                QEvent.MouseButtonRelease,
                QEvent.MouseMove,
                QEvent.Leave,
                QEvent.HoverMove,
                QEvent.MouseButtonDblClick
            )
            if e.type() in e_list:
                QApplication.sendEvent(self, e)
        return super().eventFilter(obj, e)

    def event(self, e: QEvent):
        if e and e.type() == QEvent.WindowStateChange:
            if self.isUseRibbon():
                self.m_windowButtonGroup.setWindowStates(self.windowState())
        return super().event(e)

    def loadTheme(self, filepath: str):
        qFile = QFile(filepath)
        if not qFile.open(QIODevice.ReadOnly | QIODevice.Text):
            return
        style_str = str(qFile.readAll(), encoding='utf-8')
        self.setStyleSheet(style_str)
        qFile.close()

    NormalTheme = 0
    Office2013Theme = 1
