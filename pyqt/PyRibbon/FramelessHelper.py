#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

from typing import Dict

from PyQt5.QtCore import Qt
from PyQt5.QtCore import QObject, QPoint, QRect, QEvent
from PyQt5.QtWidgets import QWidget, QRubberBand


class CursorPosCalculator:
    """
    计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角
    """
    m_nBorderWidth = 5
    m_nTitleHeight = 30

    def __init__(self):
        self.reset()

    def reset(self):
        self.m_bOnEdges = False
        self.m_bOnLeftEdge = False
        self.m_bOnRightEdge = False
        self.m_bOnTopEdge = False
        self.m_bOnBottomEdge = False
        self.m_bOnTopLeftEdge = False
        self.m_bOnBottomLeftEdge = False
        self.m_bOnTopRightEdge = False
        self.m_bOnBottomRightEdge = False

    def recalculate(self, pos: QPoint, rect: QRect):
        mouseX = pos.x()
        mouseY = pos.y()
        x = rect.x()
        y = rect.y()
        width = rect.width()
        height = rect.height()

        self.m_bOnLeftEdge = (mouseX >= x) and (mouseX <= x + self.m_nBorderWidth)
        self.m_bOnRightEdge = (mouseX >= x + width - self.m_nBorderWidth) and (mouseX <= x + width)
        self.m_bOnTopEdge = (mouseY >= y) and (mouseY <= y + self.m_nBorderWidth)
        self.m_bOnBottomEdge = (mouseY >= y + height - self.m_nBorderWidth) and (mouseY <= y + height)

        self.m_bOnTopLeftEdge = self.m_bOnTopEdge and self.m_bOnLeftEdge
        self.m_bOnBottomLeftEdge = self.m_bOnBottomEdge and self.m_bOnLeftEdge
        self.m_bOnTopRightEdge = self.m_bOnTopEdge and self.m_bOnRightEdge
        self.m_bOnBottomRightEdge = self.m_bOnBottomEdge and self.m_bOnRightEdge
        self.m_bOnEdges = self.m_bOnLeftEdge or self.m_bOnRightEdge or self.m_bOnTopEdge or self.m_bOnBottomEdge


class WidgetData:
    """更新鼠标样式、移动窗体、缩放窗体"""
    def __init__(self, mainClass, parent):  # parent: SARibbonMainWindow
        self.obj: FramelessHelper = mainClass
        self.m_pWidget: QWidget = parent
        self.m_pWidget.setMouseTracking(True)
        self.m_pWidget.setAttribute(Qt.WA_Hover, True)

        self.m_windowFlags = self.m_pWidget.windowFlags()
        self.m_bLeftButtonPressed = False
        self.m_bCursorShapeChanged = False
        self.m_bLeftButtonTitlePressed = False
        self.m_pRubberBand: QRubberBand = None
        self.m_ptDragPos = QPoint()
        self.m_pressedMousePos = CursorPosCalculator()
        self.m_moveMousePos = CursorPosCalculator()

        self.updateRubberBandStatus()

    def widget(self) -> QWidget:
        return self.m_pWidget

    def handleWidgetEvent(self, event) -> bool:
        """
        处理鼠标事件-划过、按下、释放、移动
        """
        edict = {
            QEvent.MouseButtonPress: self.handleMousePressEvent,
            QEvent.MouseButtonRelease: self.handleMouseReleaseEvent,
            QEvent.MouseMove: self.handleMouseMoveEvent,
            QEvent.Leave: self.handleLeaveEvent,
            QEvent.HoverMove: self.handleHoverMoveEvent,
            QEvent.MouseButtonDblClick: self.handleDoubleClickedMouseEvent,
        }
        etype = event.type()
        if etype in edict:
            return edict[etype](event)

        print('Unknow Event Type: %s' % etype)
        return False

    def updateRubberBandStatus(self):
        """
        更新橡皮筋状态
        """
        if self.obj.m_bRubberBandOnMove or self.obj.m_bRubberBandOnResize:
            if self.m_pRubberBand is None:
                self.m_pRubberBand = QRubberBand(QRubberBand.Rectangle)
        else:
            self.m_pRubberBand = None

    def updateCursorShape(self, pos: QPoint):
        """
        更新鼠标样式
        """
        if self.m_pWidget.isFullScreen() or self.m_pWidget.isMaximized():
            if self.m_bCursorShapeChanged:
                self.m_pWidget.unsetCursor()
            return

        self.m_moveMousePos.recalculate(pos, self.m_pWidget.frameGeometry())
        if self.m_moveMousePos.m_bOnTopLeftEdge or self.m_moveMousePos.m_bOnBottomRightEdge:
            self.m_pWidget.setCursor(Qt.SizeFDiagCursor)
            self.m_bCursorShapeChanged = True
        elif self.m_moveMousePos.m_bOnTopRightEdge or self.m_moveMousePos.m_bOnBottomLeftEdge:
            self.m_pWidget.setCursor(Qt.SizeBDiagCursor)
            self.m_bCursorShapeChanged = True
        elif self.m_moveMousePos.m_bOnLeftEdge or self.m_moveMousePos.m_bOnRightEdge:
            self.m_pWidget.setCursor(Qt.SizeHorCursor)
            self.m_bCursorShapeChanged = True
        elif self.m_moveMousePos.m_bOnTopEdge or self.m_moveMousePos.m_bOnBottomEdge:
            self.m_pWidget.setCursor(Qt.SizeVerCursor)
            self.m_bCursorShapeChanged = True
        elif self.m_bCursorShapeChanged:
            self.m_pWidget.unsetCursor()
            self.m_bCursorShapeChanged = False

    def resizeWidget(self, pos: QPoint):
        """窗口重置大小"""
        if self.obj.m_bRubberBandOnResize:
            origRect = self.m_pRubberBand.frameGeometry()
        else:
            origRect = self.m_pWidget.frameGeometry()

        left, top, right, bottom = origRect.getCoords()
        minWidth = self.m_pWidget.minimumWidth()
        minHeight = self.m_pWidget.minimumHeight()

        if self.m_pressedMousePos.m_bOnTopLeftEdge:
            left = pos.x()
            top = pos.y()
        elif self.m_pressedMousePos.m_bOnBottomLeftEdge:
            left = pos.x()
            bottom = pos.y()
        elif self.m_pressedMousePos.m_bOnTopRightEdge:
            right = pos.x()
            top = pos.y()
        elif self.m_pressedMousePos.m_bOnBottomRightEdge:
            right = pos.x()
            bottom = pos.y()
        elif self.m_pressedMousePos.m_bOnLeftEdge:
            left = pos.x()
        elif self.m_pressedMousePos.m_bOnRightEdge:
            right = pos.x()
        elif self.m_pressedMousePos.m_bOnTopEdge:
            top = pos.y()
        elif self.m_pressedMousePos.m_bOnBottomEdge:
            bottom = pos.y()

        newRect = QRect(QPoint(left, top), QPoint(right, bottom))
        if newRect.isValid():
            if minWidth > newRect.width():
                if left != origRect.left():
                    newRect.setLeft(origRect.left())
                else:
                    newRect.setRight(origRect.right())
            if minHeight > newRect.height():
                if top != origRect.top():
                    newRect.setTop(origRect.top())
                else:
                    newRect.setBottom(origRect.bottom())

            if self.obj.m_bRubberBandOnResize:
                self.m_pRubberBand.setGeometry(newRect)
            else:
                self.m_pWidget.setGeometry(newRect)

    def moveWidget(self, pos: QPoint):
        """移动窗口"""
        if self.obj.m_bRubberBandOnMove:
            self.m_pRubberBand.move(pos - self.m_ptDragPos)
        else:
            self.m_pWidget.move(pos - self.m_ptDragPos)

    def handleMousePressEvent(self, event) -> bool:
        if event.button() == Qt.LeftButton:
            self.m_bLeftButtonPressed = True
            self.m_bLeftButtonTitlePressed = (event.pos().y() < self.m_moveMousePos.m_nTitleHeight)

            frameRect = self.m_pWidget.frameGeometry()
            self.m_pressedMousePos.recalculate(event.globalPos(), frameRect)
            self.m_ptDragPos = event.globalPos() - frameRect.topLeft()

            if self.m_pressedMousePos.m_bOnEdges:
                # 窗口在最大化状态时，点击标题栏不做任何处理
                if self.m_pWidget.isMaximized():
                    return False
                if self.obj.m_bRubberBandOnResize:
                    self.m_pRubberBand.setGeometry(frameRect)
                    self.m_pRubberBand.show()
                    return True
            elif self.obj.m_bRubberBandOnMove and self.m_bLeftButtonTitlePressed:
                # 窗口在最大化状态时，点击标题栏不做任何处理
                if self.m_pWidget.isMaximized():
                    return False
                self.m_pRubberBand.setGeometry(frameRect)
                self.m_pRubberBand.show()
                return True
        return False

    def handleMouseReleaseEvent(self, event) -> bool:
        if event.button() == Qt.LeftButton:
            self.m_bLeftButtonPressed = False
            self.m_bLeftButtonTitlePressed = False
            self.m_pressedMousePos.reset()
            if self.m_pRubberBand and self.m_pRubberBand.isVisible():
                self.m_pRubberBand.hide()
                self.m_pWidget.setGeometry(self.m_pRubberBand.geometry())
                return True
        return False

    def handleMouseMoveEvent(self, event) -> bool:
        if self.m_bLeftButtonPressed:
            if self.obj.m_bWidgetResizable and self.m_pressedMousePos.m_bOnEdges:
                if self.m_pWidget.isMaximized():
                    # 窗口在最大化状态时，点击边界不做任何处理
                    return False
                self.resizeWidget(event.globalPos())
                return True
            elif self.obj.m_bWidgetMovable and self.m_bLeftButtonTitlePressed:
                if self.m_pWidget.isMaximized():
                    # 先求出窗口到鼠标的相对位置
                    normalGeometry = self.m_pWidget.normalGeometry()
                    self.m_pWidget.showNormal()
                    # p = event.globalPos()
                    ry = event.globalY()
                    rx = event.globalX()
                    ry -= 10
                    rx -= (normalGeometry.width() / 2)
                    self.m_pWidget.move(int(rx), int(ry))
                    # 这时要重置m_ptDragPos
                    self.m_ptDragPos = QPoint(int(normalGeometry.width() / 2), 10)
                    return True

                self.moveWidget(event.globalPos())
                return True
            return False
        elif self.obj.m_bWidgetResizable:
            self.updateCursorShape(event.globalPos())
        return False

    def handleLeaveEvent(self, event) -> bool:
        if not self.m_bLeftButtonPressed:
            self.m_pWidget.unsetCursor()
            return True
        return False

    def handleHoverMoveEvent(self, event) -> bool:
        if self.obj.m_bWidgetResizable:
            self.updateCursorShape(self.m_pWidget.mapToGlobal(event.pos()))
        return False

    def handleDoubleClickedMouseEvent(self, event) -> bool:
        if event.button() == Qt.LeftButton:
            if self.m_pWidget and (self.m_pWidget.windowButtonFlags() & Qt.WindowMaximizeButtonHint):
                # 在最大化按钮显示时才进行shownormal处理
                if event.pos().y() < self.m_moveMousePos.m_nTitleHeight:
                    if self.m_pWidget.isMaximized():
                        self.m_pWidget.showNormal()
                    else:
                        self.m_pWidget.showMaximized()
                    return True
        return False


class FramelessHelper(QObject):
    """设置主类窗体效果"""
    def __init__(self, parent):
        super().__init__(parent)

        # 存储界面对应的数据集合，以及是否可移动、可缩放属性
        self.m_bWidgetMovable = True
        self.m_bWidgetResizable = True
        self.m_bRubberBandOnMove = False
        self.m_bRubberBandOnResize = False
        self.m_widgetDataHash: Dict[QWidget, WidgetData] = dict()

        if parent:
            w: QWidget = parent
            w.setWindowFlags(w.windowFlags() | Qt.FramelessWindowHint)  # 设置无边框
            self.activateOn(w)  # 激活当前窗体

    def activateOn(self, parent: QWidget):
        """激活窗体事件"""
        if parent not in self.m_widgetDataHash:
            data = WidgetData(self, parent)
            self.m_widgetDataHash[parent] = data
            parent.installEventFilter(self)

    def removeFrom(self, parent: QWidget):
        """移除窗体事件"""
        data = self.m_widgetDataHash.get(parent, None)
        if data:
            self.m_widgetDataHash.pop(parent)
            parent.removeEventFilter(self)

    def setWidgetMovable(self, movable: bool):
        """设置窗体移动"""
        self.m_bWidgetMovable = movable

    def setWidgetResizable(self, resizable: bool):
        """设置窗体缩放"""
        self.m_bWidgetResizable = resizable

    def setRubberBandOnMove(self, movable: bool):
        """设置橡皮筋移动"""
        self.m_bRubberBandOnMove = movable
        dataList = self.m_widgetDataHash.values()
        for data in dataList:
            data.updateRubberBandStatus()

    def setRubberBandOnResize(self, resizable: bool):
        """设置橡皮筋缩放"""
        self.m_bRubberBandOnResize = resizable
        dataList = self.m_widgetDataHash.values()
        for data in dataList:
            data.updateRubberBandStatus()

    def setBorderWidth(self, width: int):
        """设置边框的宽度"""
        if width > 0:
            CursorPosCalculator.m_nBorderWidth = width

    def setTitleHeight(self, height: int):
        """设置标题栏高度"""
        if height > 0:
            CursorPosCalculator.m_nTitleHeight = height

    def widgetResizable(self) -> bool:
        return self.m_bWidgetResizable

    def widgetMovable(self) -> bool:
        return self.m_bWidgetMovable

    def rubberBandOnMove(self) -> bool:
        return self.m_bRubberBandOnMove

    def rubberBandOnResisze(self) -> bool:
        return self.m_bRubberBandOnResize

    def borderWidth(self) -> int:
        return CursorPosCalculator.m_nBorderWidth

    def titleHeight(self) -> int:
        return CursorPosCalculator.m_nTitleHeight

    def eventFilter(self, obj, event: QEvent):
        filter_list = (
            QEvent.MouseMove,
            QEvent.HoverMove,
            QEvent.MouseButtonPress,
            QEvent.MouseButtonRelease,
            QEvent.MouseButtonDblClick,
            QEvent.Leave,
        )
        if event.type() in filter_list:
            data = self.m_widgetDataHash.get(obj, None)
            if data:
                return data.handleWidgetEvent(event)
        return super().eventFilter(obj, event)


if __name__ == '__main__':
    from PyQt5.QtWidgets import QApplication

    app = QApplication([])
    mainWindow = QWidget()

    framelessHelper = FramelessHelper(mainWindow)

    mainWindow.setMinimumWidth(500)
    mainWindow.show()
    app.exec()
