#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

from PyQt5.QtCore import Qt, QSize
from PyQt5.QtCore import QEvent
from PyQt5.QtWidgets import QWidget, QPushButton


class WindowToolButton(QPushButton):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFlat(True)


class WindowButtonGroup(QWidget):

    def __init__(self, parent, flags=None):
        super().__init__(parent)
        self.m_closeStretch = 4
        self.m_maxStretch = 3
        self.m_minStretch = 3
        self.m_iconScale = 0.5
        self.m_winFlags = Qt.Widget
        self.m_closeButton = None
        self.m_minimizeButton = None
        self.m_maximizeButton = None

        if flags:
            self.m_winFlags = flags
        self.updateWindowFlag()
        if parent:
            parent.installEventFilter(self)

    def setupMinimizeButton(self, on: bool):
        if on:
            if not self.m_minimizeButton:
                self.m_minimizeButton = WindowToolButton(self)
                self.m_minimizeButton.setObjectName('qx_MinimizeWindowButton')
                self.m_minimizeButton.setFixedSize(30, 28)
                self.m_minimizeButton.setFocusPolicy(Qt.NoFocus)
                self.m_minimizeButton.clicked.connect(self.onMinimizeWindow)
            self.m_minimizeButton.setIconSize(self.m_minimizeButton.size() * self.m_iconScale)
            self.m_minimizeButton.show()
        elif self.m_minimizeButton:
            self.m_minimizeButton.hide()
        self.updateSize()

    def setupMaximizeButton(self, on: bool):
        if on:
            if not self.m_maximizeButton:
                self.m_maximizeButton = WindowToolButton(self)
                self.m_maximizeButton.setObjectName('qx_MaximizeWindowButton')
                self.m_maximizeButton.setFixedSize(30, 28)
                self.m_maximizeButton.setFocusPolicy(Qt.NoFocus)
                self.m_maximizeButton.clicked.connect(self.onMaximizeWindow)
            self.m_maximizeButton.setIconSize(self.m_maximizeButton.size() * self.m_iconScale)
            self.m_maximizeButton.show()
        elif self.m_maximizeButton:
            self.m_maximizeButton.hide()
        self.updateSize()

    def setupCloseButton(self, on: bool):
        if on:
            if not self.m_closeButton:
                self.m_closeButton = WindowToolButton(self)
                self.m_closeButton.setObjectName('qx_CloseWindowButton')
                self.m_closeButton.setFixedSize(40, 28)
                self.m_closeButton.setFocusPolicy(Qt.NoFocus)
                self.m_closeButton.clicked.connect(self.onCloseWindow)
            self.m_closeButton.setIconSize(self.m_closeButton.size() * self.m_iconScale)
            self.m_closeButton.show()
        elif self.m_closeButton:
            self.m_closeButton.hide()
        self.updateSize()

    def updateWindowFlag(self, flags=None):
        if flags:
            if flags & Qt.WindowCloseButtonHint:
                self.m_winFlags |= Qt.WindowCloseButtonHint
            else:
                self.m_winFlags &= ~Qt.WindowCloseButtonHint
            if flags & Qt.WindowMaximizeButtonHint:
                self.m_winFlags |= Qt.WindowMaximizeButtonHint
            else:
                self.m_winFlags &= ~Qt.WindowMaximizeButtonHint
            if flags & Qt.WindowMinimizeButtonHint:
                self.m_winFlags |= Qt.WindowMinimizeButtonHint
            else:
                self.m_winFlags &= ~Qt.WindowMinimizeButtonHint
        else:
            flags = self.parentWidget().windowFlags()
            self.m_winFlags = flags

        self.setupMinimizeButton(flags & Qt.WindowMinimizeButtonHint)
        self.setupMaximizeButton(flags & Qt.WindowMaximizeButtonHint)
        self.setupCloseButton(flags & Qt.WindowCloseButtonHint)

    def setButtonWidthStretch(self, close_st=4, max_st=3, min_st=3):
        self.m_closeStretch = close_st
        self.m_maxStretch = max_st
        self.m_minStretch = min_st

    def setIconScale(self, icon_scale=0.5):
        self.m_iconScale = icon_scale

    def setWindowStates(self, states: Qt.WindowStates):
        if self.m_maximizeButton:
            on = (states & Qt.WindowMaximized == Qt.WindowMaximized)
            self.m_maximizeButton.setChecked(on)
            self.m_maximizeButton.setToolTip(self.tr("Restore") if on else self.tr("Maximize"))

    def windowButtonFlags(self) -> int:
        f = Qt.Widget   # Qt::widget is 0
        if self.m_winFlags & Qt.WindowCloseButtonHint:
            f |= Qt.WindowCloseButtonHint
        if self.m_winFlags & Qt.WindowMaximizeButtonHint:
            f |= Qt.WindowMaximizeButtonHint
        if self.m_winFlags & Qt.WindowMinimizeButtonHint:
            f |= Qt.WindowMinimizeButtonHint
        return f

    def parentResize(self):
        par = self.parentWidget()
        if par:
            pSize = par.size()
            self.move(pSize.width() - self.width() - 1, 1)

    def updateSize(self):
        self.setFixedSize(self.sizeHint())
        self.resize(self.size())

    def resize(self, size: QSize):
        tw = 0
        if self.m_closeButton:
            tw += self.m_closeStretch
        if self.m_maximizeButton:
            tw += self.m_maxStretch
        if self.m_minimizeButton:
            tw += self.m_minStretch

        # Affected by the QSS style, the minimum width and height cannot be set,
        # so it is necessary to set fixed size before moving
        x = 0
        if self.m_minimizeButton:
            w = (self.m_minStretch / tw) * size.width()
            # self.m_minimizeButton.setGeometry(x, 0, w, size.height())
            self.m_minimizeButton.setFixedSize(int(w), size.height())
            self.m_minimizeButton.move(x, 0)
            x += w
        if self.m_maximizeButton:
            w = (self.m_maxStretch / tw) * size.width()
            # self.m_maximizeButton.setGeometry(x, 0, w, size.height())
            self.m_maximizeButton.setFixedSize(int(w), size.height())
            self.m_maximizeButton.move(int(x), 0)
            x += w
        if self.m_closeButton:
            w = (self.m_closeStretch / tw) * size.width()
            # self.m_closeButton.setGeometry(x, 0, w, size.height())
            self.m_closeButton.setFixedSize(int(w), size.height())
            self.m_closeButton.move(int(x), 0)

    def sizeHint(self) -> QSize:
        w = 0
        h = 28

        if self.m_closeButton:
            w += 40
        if self.m_maximizeButton:
            w += 30
        if self.m_minimizeButton:
            w += 30
        return QSize(w, h)

    def eventFilter(self, watched, e: QEvent) -> bool:
        if watched == self.parentWidget():
            if e.type() == QEvent.Resize:
                self.parentResize()
        return False    # 不截断任何事件

    def resizeEvent(self, e):
        self.resize(e.size())

    def onCloseWindow(self):
        try:
            if self.parentWidget():
                self.parentWidget().close()
        except Exception as e:
            print(__file__, 'onCloseWindow:', e)

    def onMinimizeWindow(self):
        if self.parentWidget():
            self.parentWidget().showMinimized()

    def onMaximizeWindow(self):
        par = self.parentWidget()
        if par:
            if par.isMaximized():
                par.showNormal()
            else:
                par.showMaximized()
