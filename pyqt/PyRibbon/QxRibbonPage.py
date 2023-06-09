#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

from typing import List, Union

from PyQt5.QtCore import QSize, Qt, QEvent, QObject, QRect, QMargins, QPoint, pyqtSignal, QVariant
from PyQt5.QtGui import QBrush, QPalette, QWheelEvent, QResizeEvent, QColor
from PyQt5.QtWidgets import QWidget, QToolButton

from .QxRibbonGroup import RibbonGroup
from .QxRibbonControls import RibbonSeparator


class RibbonPageItem:
    def __init__(self):
        self.m_group: RibbonGroup = None
        self.m_separator: RibbonSeparator = None
        self.m_willSetGroupGeometry = QRect()
        self.m_willSetSeparatorGeometry = QRect()

    def isEmpty(self) -> bool:
        if self.m_group:
            return self.m_group.isHidden()
        return True

    def isNull(self) -> bool:
        return self.m_group is None


class RibbonPageScrollButton(QToolButton):
    def __init__(self, arr: int, parent=None):
        super().__init__(parent)
        self.setArrowType(arr)


class RibbonPage(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_groupLayoutMode = RibbonGroup.ThreeRowMode
        self.m_itemList: List[RibbonPageItem] = list()
        self.m_sizeHint = QSize(50, 50)
        self.m_contentsMargins = QMargins(1, 1, 1, 1)
        self.m_totalWidth = 0
        self.m_XBase = 0
        self.m_isRightScrollBtnShow = False
        self.m_isLeftScrollBtnShow = False
        self.m_isPageContext = False
        self.m_isCanCustomize = True

        self.m_leftScrollBtn = RibbonPageScrollButton(Qt.LeftArrow, self)
        self.m_rightScrollBtn = RibbonPageScrollButton(Qt.RightArrow, self)
        self.m_leftScrollBtn.setVisible(False)
        self.m_rightScrollBtn.setVisible(False)
        self.m_leftScrollBtn.clicked.connect(self.onLeftScrollButtonClicked)
        self.m_rightScrollBtn.clicked.connect(self.onRightScrollButtonClicked)

        self.setAutoFillBackground(True)
        self.setBackgroundBrush(Qt.white)

    def pageName(self) -> str:
        return self.windowTitle()

    def setPageName(self, title: str):
        self.setWindowTitle(title)

    def groupLayoutMode(self) -> int:
        return self.m_groupLayoutMode

    def addGroup(self, *args) -> Union[None, RibbonGroup]:
        """
        addGroup(title: str) -> RibbonGroup
        addGroup(group: RibbonGroup) -> None
        """
        if len(args) == 1:
            if isinstance(args[0], str):
                return self.insertGroup(len(self.m_itemList), args[0])
            else:
                return self._insertGroup(len(self.m_itemList), args[0])

    def _insertGroup(self, index: int, group: RibbonGroup):
        if group.parentWidget() != self:
            group.setParent(self)
        group.setGroupLayoutMode(self.m_groupLayoutMode)
        group.installEventFilter(self)
        index = max(0, index)
        index = min(len(self.m_itemList), index)
        item = RibbonPageItem()
        item.m_group = group
        item.m_separator = RibbonSeparator(self)
        self.m_itemList.insert(index, item)
        self.updateItemGeometry()
        group.setVisible(True)

    def insertGroup(self, index: int, title: str):
        group = RibbonGroup(self)
        group.setWindowTitle(title)
        group.setObjectName(title)
        self._insertGroup(index, group)
        return group

    def group(self, index: int) -> RibbonGroup:
        return self.m_itemList[index].m_group

    def groupByName(self, title: str) -> RibbonGroup:
        for item in self.m_itemList:
            if item.m_group:
                if item.m_group.windowTitle() == title:
                    return item.m_group
        return None

    def groupByObjectName(self, objname: str) -> RibbonGroup:
        for item in self.m_itemList:
            if item.m_group:
                if item.m_group.objectName() == objname:
                    return item.m_group
        return None

    def groupIndex(self, group: RibbonGroup) -> int:
        for i, item in enumerate(self.m_itemList):
            if item.m_group == group:
                return i
        return -1

    def moveGroup(self, fr: int, to: int):
        item = self.m_itemList.pop(fr)
        self.m_itemList.insert(to, item)
        self._updateItemGeometry()

    def takeGroup(self, group: RibbonGroup) -> bool:
        item = None
        for i, item in enumerate(self.m_itemList):
            if item.m_group == group:
                item = self.m_itemList.pop(i)
                break
        if not item or item.isNull():
            return False
        if item.m_separator:
            item.m_separator.hide()
            item.m_separator.deleteLater()
        return True

    def removeGroup(self, *args) -> bool:
        """
        removeGroup(group: RibbonGroup) -> bool
        removeGroup(index: int) -> bool
        """
        group = None
        if len(args) == 1:
            if isinstance(args[0], int):
                group = self.group(args[0])
            else:
                group = args[0]
        if not group:
            return False
        if self.takeGroup(group):
            return True
        return False

    def groupCount(self) -> int:
        return len(self.m_itemList)

    def groupList(self) -> List[RibbonGroup]:
        res = []
        for item in self.m_itemList:
            if not item.isNull():
                res.append(item.m_group)
        return res

    def setBackgroundBrush(self, brush: QBrush):
        palette = self.palette()
        palette.setBrush(QPalette.Window, brush)
        self.setPalette(palette)

    def sizeHint(self) -> QSize:
        return self.m_sizeHint

    def isPageContext(self) -> bool:
        return self.m_isPageContext

    def isCanCustomize(self) -> bool:
        return self.m_isCanCustomize

    def setCanCustomize(self, b: bool):
        self.m_isCanCustomize = b

    def updateItemGeometry(self):
        for group in self.groupList():
            group.updateGeometry()
        self._updateItemGeometry()

    def setGroupLayoutMode(self, m: int):
        if self.m_groupLayoutMode == m:
            return
        self.m_groupLayoutMode = m
        for group in self.groupList():
            group.setGroupLayoutMode(m)
        self.updateItemGeometry()

    def markIsPageContext(self, b: bool = True):
        self.m_isPageContext = b

    def event(self, event: QEvent) -> bool:
        if event.type() == QEvent.LayoutRequest:
            self._updateItemGeometry()
        return super().event(event)

    def resizeEvent(self, event: QResizeEvent):
        super().resizeEvent(event)
        self._updateItemGeometry()

    def eventFilter(self, watched: QObject, event: QEvent) -> bool:
        return False

    def wheelEvent(self, event: QWheelEvent):
        contentSize = self._pageContentSize()
        totalWidth = self.m_totalWidth
        if totalWidth > contentSize.width():
            numPixels: QPoint = event.pixelDelta()
            numDegrees: QPoint = event.angleDelta() / 8
            scrollPix = 0
            if not numPixels.isNull():
                scrollPix = numPixels.x() / 4
            elif not numDegrees.isNull():
                scrollPix = numDegrees.x() / 15
            if scrollPix > 0:
                tmp = self.m_XBase - scrollPix
                if tmp < (contentSize.width() - totalWidth):
                    tmp = contentSize.width() - totalWidth
                self.m_XBase = tmp
            else:
                tmp = self.m_XBase - scrollPix
                if tmp > 0:
                    tmp = 0
                self.m_XBase = tmp
            self._updateItemGeometry()
        else:
            event.ignore()
            self.m_XBase = 0

    def onLeftScrollButtonClicked(self):
        width = self._pageContentSize().width()
        if self.m_totalWidth > width:
            tmp = self.m_XBase + width
            if tmp > 0:
                tmp = 0
            self.m_XBase = tmp
        else:
            self.m_XBase = 0
        self._updateItemGeometry()

    def onRightScrollButtonClicked(self):
        width = self._pageContentSize().width()
        totalWidth = self.m_totalWidth
        if totalWidth > width:
            tmp = self.m_XBase - width
            if tmp < (width - totalWidth):
                tmp = width - totalWidth
            self.m_XBase = tmp
        else:
            self.m_XBase = 0
        self._updateItemGeometry()

    def _updateItemGeometry(self):
        contentSize = self._pageContentSize()
        y = 0
        if not self.m_contentsMargins.isNull():
            y = self.m_contentsMargins.top()
        total = self._totalSizeHintWidth()
        canExpandingCount = expandWidth = 0
        if total <= contentSize.width():
            self.m_XBase = 0
            for item in self.m_itemList:
                if not item.isEmpty():
                    if item.m_group.isExpanding():
                        canExpandingCount += 1
            if canExpandingCount > 0:
                expandWidth = round((contentSize.width() - total) / canExpandingCount)
            else:
                expandWidth = 0
        total = 0
        x = self.m_XBase
        for item in self.m_itemList:
            if item.isEmpty():
                if item.m_separator:
                    item.m_separator.hide()
                item.m_willSetGroupGeometry = QRect(0, 0, 0, 0)
                item.m_willSetSeparatorGeometry = QRect(0, 0, 0, 0)
                continue
            group = item.m_group
            if not group:
                print("unknow widget in RibbonPageLayout")
                continue
            groupSize = group.sizeHint()
            separatorSize = QSize(0, 0)
            if item.m_separator:
                separatorSize = item.m_separator.sizeHint()
            if group.isExpanding():
                groupSize.setWidth(groupSize.width() + expandWidth)
            w = groupSize.width()
            item.m_willSetGroupGeometry = QRect(x, y, w, contentSize.height())
            x += w
            total += w
            w = separatorSize.width()
            item.m_willSetSeparatorGeometry = QRect(x, y, w, contentSize.height())
            x += w
            total += w
        self.m_totalWidth = total
        if total > contentSize.width():
            if self.m_XBase == 0:
                self.m_isRightScrollBtnShow = True
                self.m_isLeftScrollBtnShow = False
            elif self.m_XBase <= (contentSize.width() - total):
                self.m_isRightScrollBtnShow = False
                self.m_isLeftScrollBtnShow = True
            else:
                self.m_isRightScrollBtnShow = True
                self.m_isLeftScrollBtnShow = True
        else:
            self.m_isRightScrollBtnShow = False
            self.m_isLeftScrollBtnShow = False
        parent = self.parentWidget()
        parentHeight = parent.height() if parent else contentSize.height()
        parentWidth = parent.width() if parent else total
        self.m_sizeHint = QSize(parentWidth, parentHeight)
        self._doItemLayout()

    def _doItemLayout(self):
        self.m_leftScrollBtn.setGeometry(0, 0, 12, self.height())
        self.m_rightScrollBtn.setGeometry(self.width() - 12, 0, 12, self.height())
        showWidgets = []
        hideWidgets = []
        for item in self.m_itemList:
            if item.isNull():
                continue
            if item.isEmpty():
                hideWidgets.append(item.m_group)
                if item.m_separator:
                    hideWidgets.append(item.m_separator)
            else:
                item.m_group.setGeometry(item.m_willSetGroupGeometry)
                showWidgets.append(item.m_group)
                if item.m_separator:
                    item.m_separator.setGeometry(item.m_willSetSeparatorGeometry)
                    showWidgets.append(item.m_separator)
        self.m_rightScrollBtn.setVisible(self.m_isRightScrollBtnShow)
        self.m_leftScrollBtn.setVisible(self.m_isLeftScrollBtnShow)
        if self.m_isRightScrollBtnShow:
            self.m_rightScrollBtn.raise_()
        if self.m_isLeftScrollBtnShow:
            self.m_leftScrollBtn.raise_()
        for w in showWidgets:
            w.show()
        for w in hideWidgets:
            w.hide()

    def _pageContentSize(self) -> QSize:
        s = self.size()
        m = self.m_contentsMargins
        if not m.isNull():
            s.setHeight(s.height() - m.top() - m.bottom())
            s.setWidth(s.width() - m.right() - m.left())
        return s

    def _totalSizeHintWidth(self) -> int:
        total = 0
        m = self.m_contentsMargins
        if not m.isNull():
            total += m.left() + m.right()
        for item in self.m_itemList:
            if item.isEmpty():
                continue
            groupSize = item.m_group.sizeHint()
            separatorSize = QSize(0, 0)
            if item.m_separator:
                separatorSize = item.m_separator.sizeHint()
            total += groupSize.width() + separatorSize.width()
        return total


class RibbonPageContext(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_pageList: List[RibbonPage] = list()
        self.m_contextID: QVariant = QVariant()
        self.m_contextColor: QColor = QColor()
        self.m_contextTitle: str = ""

    def addPage(self, title: str) -> RibbonPage:
        page = RibbonPage(self.parentWidget())
        page.markIsPageContext(True)
        page.setWindowTitle(title)
        page.installEventFilter(self)
        self.m_pageList.append(page)
        self.pageAdded.emit(page)
        return page

    def page(self, index: int) -> RibbonPage:
        if index < 0 or index >= len(self.m_pageList):
            return None
        return self.m_pageList[index]

    def pageCount(self) -> int:
        return len(self.m_pageList)

    def pageList(self) -> List[RibbonPage]:
        return self.m_pageList

    def takePage(self, page: RibbonPage) -> bool:
        for i, v in enumerate(self.m_pageList):
            if v == page:
                self.m_pageList.pop(i)
                return True
        return False

    def getId(self) -> QVariant:
        return self.m_contextID

    def setId(self, i: QVariant):
        self.m_contextID = i

    def contextColor(self) -> QColor:
        return self.m_contextColor

    def setContextColor(self, color: QColor):
        self.m_contextColor = color

    def contextTitle(self) -> str:
        return self.m_contextTitle

    def setContextTitle(self, title: str):
        self.m_contextTitle = title

    def eventFilter(self, watched: QObject, event: QEvent) -> bool:
        if not watched:
            return False
        if event.type() == QEvent.Close:
            if isinstance(watched, RibbonPage):
                page: RibbonPage = watched
                self.takePage(page)
        return False

    def parentWidget(self) -> QWidget:
        w: QWidget = self.parent()
        return w

    # signals
    pageAdded = pyqtSignal(RibbonPage)
    pageRemoved = pyqtSignal(RibbonPage)
