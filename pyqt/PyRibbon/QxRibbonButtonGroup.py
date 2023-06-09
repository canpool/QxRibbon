#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

from typing import List

from PyQt5.QtCore import Qt, QSize, pyqtSignal, QEvent
from PyQt5.QtGui import QActionEvent
from PyQt5.QtWidgets import QFrame, QAction, QMenu, QToolButton, QWidget, \
    QHBoxLayout, QSizePolicy, QWidgetAction

from .QxRibbonButton import RibbonButton
from .QxRibbonControls import RibbonSeparator


class RibbonButtonGroupItem:

    def __init__(self, *args):
        """
        RibbonButtonGroupItem()
        RibbonButtonGroupItem(a: QAction, w: QWidget, cw: bool)
        """
        if len(args) < 3:
            self.action: QAction = None
            self.widget: QWidget = None
            self.customWidget: bool = False
        else:
            self.action: QAction = args[0]
            self.widget: QWidget = args[1]
            self.customWidget: bool = args[2]

    def compare(self, *args) -> bool:
        """
        compare(action: QAction) -> bool
        compare(w: RibbonButtonGroupItem) -> bool
        """
        if len(args) < 1:
            return False
        if isinstance(args[0], QAction):
            return self.action == args[0]
        elif isinstance(args[0], RibbonButtonGroupItem):
            return self.action == args[0].action
        else:
            return False


class RibbonButtonGroup(QFrame):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_items: List[RibbonButtonGroupItem] = list()

        layout = QHBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        self.setLayout(layout)
        self.setSizePolicy(QSizePolicy.Maximum, QSizePolicy.Maximum)

    def addAction(self, *args) -> QAction:
        """
        addAction(a: QAction) -> QAction
        addAction(text: str, icon: QIcon, pop_mode=QToolButton.InstantPopup) -> QAction
        """
        if len(args) == 1:
            super().addAction(args[0])
            return args[0]
        else:
            a = QAction(args[1], args[0], self)
            super().addAction(a)
            pop_mode = QToolButton.InstantPopup if len(args) < 3 else args[2]
            if self.m_items:
                button: RibbonButton = self.m_items[-1].widget
                button.setPopupMode(pop_mode)
            return a

    def addMenu(self, menu: QMenu, pop_mode=QToolButton.InstantPopup) -> QAction:
        a = menu.menuAction()
        self.addAction(a)
        btn = self.m_items[-1].widget
        btn.setPopupMode(pop_mode)
        return a

    def addSeparator(self) -> QAction:
        a = QAction(self)
        a.setSeparator(True)
        self.addAction(a)
        return a

    def addWidget(self, w: QWidget):
        a = QWidgetAction(self)
        a.setDefaultWidget(w)
        w.setAttribute(Qt.WA_Hover)
        self.addAction(a)
        return a

    def hideWidget(self, action: QAction):
        i = len(self.m_items)
        for i, it in enumerate(self.m_items):
            if isinstance(it.action, QWidgetAction) and it.action == action:
                it.widget.hide()
                widgetAction: QWidgetAction = it.action
                widgetAction.releaseWidget(it.widget)
                break
        if i < len(self.m_items):
            self.m_items.pop(i)

    def sizeHint(self) -> QSize:
        return self.layout().sizeHint()

    def minimumSizeHint(self) -> QSize:
        return self.layout().minimumSize()

    def actionEvent(self, e: QActionEvent):
        item = RibbonButtonGroupItem()
        item.action = e.action()

        if e.type() == QEvent.ActionAdded:
            if isinstance(item.action, QWidgetAction):
                item.action.setParent(self)
                widgetAction: QWidgetAction = item.action
                """here widgetAction.requestWidget(self) is not widgetAction.defaultWidget()
                if using requestWidget will cause defaultWidget was deleted by python garbage collect
                see QWidgetAction source code, using defaultWidget() and set widget parent as self.
                """
                # item.widget = widgetAction.requestWidget(self)
                item.widget = widgetAction.defaultWidget()
                if item.widget:
                    item.widget.setParent(self)
                    item.widget.setAttribute(Qt.WA_LayoutUsesWidgetRect)
                    item.widget.show()
                    item.customWidget = True
            elif item.action.isSeparator():
                sp = RibbonSeparator(self)
                sp.setTopBottomMargins(3, 3)
                item.widget = sp
            if not item.widget:
                btn = RibbonButton(self)
                btn.setAutoRaise(True)
                btn.setFocusPolicy(Qt.NoFocus)
                btn.setButtonType(RibbonButton.SmallButton)
                btn.setToolButtonStyle(Qt.ToolButtonIconOnly)
                btn.setDefaultAction(item.action)
                btn.triggered.connect(self.actionTriggered)
                item.widget = btn
            self.layout().addWidget(item.widget)
            self.m_items.append(item)
        elif e.type() == QEvent.ActionChanged:
            self.layout().invalidate()
        elif e.type() == QEvent.ActionRemoved:
            # FIXME: whay clear all items?
            item.action.disconnect()
            for it in self.m_items:
                if isinstance(it.action, QWidgetAction) and it.customWidget:
                    widgetAction: QWidgetAction = it.action
                    widgetAction.releaseWidget(it.widget)
                else:
                    it.widget.hide()
                    it.widget.deleteLater()
            self.m_items.clear()
            self.layout().invalidate()

    # signals
    actionTriggered = pyqtSignal(QAction)
