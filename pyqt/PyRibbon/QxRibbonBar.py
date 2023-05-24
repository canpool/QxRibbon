#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

from typing import List, Union

from PyQt5.QtCore import Qt, QSize, pyqtSignal, QEvent, QObject, QVariant, QRect, \
    QPoint, QMargins
from PyQt5.QtGui import QColor, QPaintEvent, QResizeEvent, QMoveEvent, QIcon, QPainter, \
    QMouseEvent, QHoverEvent, QCursor, QPen
from PyQt5.QtWidgets import QMenuBar, QAbstractButton, QFrame, QAction, \
    QStackedWidget, QApplication, QStyle

from .QxRibbonTabBar import RibbonTabBar
from .QxRibbonPage import RibbonPage, RibbonPageContext
from .QxRibbonGroup import RibbonGroup, RibbonGroupLayout
from .QxRibbonButtonGroup import RibbonButtonGroup
from .QxRibbonQuickAccessBar import RibbonQuickAccessBar
from .QxRibbonApplicationButton import RibbonApplicationButton
from .QxRibbonControls import RibbonControlButton


class RibbonStackedWidget(QStackedWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_isPopup = False

    def isPopup(self) -> bool:
        return self.m_isPopup

    def setPopup(self, popup: bool):
        if self.m_isPopup == popup:
            return
        self.m_isPopup = popup
        if popup:
            self.setWindowFlags(Qt.Popup)
        else:
            self.setWindowFlags(Qt.Widget)

    def moveWidget(self, fr: int, to: int):
        w = self.widget(fr)
        self.removeWidget(w)
        self.insertWidget(to, w)

    def event(self, event: QEvent) -> bool:
        if self.m_isPopup:
            if event.type() == QEvent.Show:
                self.aboutToShow.emit()
            elif event.type() == QEvent.Hide:
                self.aboutToHide.emit()
        return super().event(event)

    aboutToShow = pyqtSignal()
    aboutToHide = pyqtSignal()


class _PageContextManagerData:
    def __init__(self):
        self.pageContext: RibbonPageContext = None
        self.tabPageIndex: List[int] = list()

    def compare(self, context: RibbonPageContext):
        return self.pageContext == context


class _RibbonTabData:
    def __init__(self):
        self.page: RibbonPage = None
        self.index: int = -1


class RibbonBar(QMenuBar):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_iconRightBorderPosition: int = 1
        self.m_minimumPageButton: RibbonControlButton = None
        self.m_rightButtonGroup: RibbonButtonGroup = None
        self.m_ribbonStyle = RibbonBar.OfficeStyle
        self.m_lastShowStyle = RibbonBar.OfficeStyle
        self.m_titleTextColor: QColor = QColor(Qt.black)
        self.m_tabBarBaseLineColor: QColor = QColor(186, 201, 219)
        self.m_titleAlignment = Qt.AlignCenter
        self.m_minimized = True
        self.m_pageContextColorListIndex = -1
        self.m_pageContextColorList: List[QColor] = [
            QColor(201, 89, 156),  # 玫红
            QColor(242, 203, 29),  # 黄
            QColor(255, 157, 0),  # 橙
            QColor(14, 81, 167),  # 蓝
            QColor(228, 0, 69),  # 红
            QColor(67, 148, 0),  # 绿
        ]
        self.m_currentShowingPageContextList: List[_PageContextManagerData] = list()
        self.m_pageContextList: List[RibbonPageContext] = list()
        self.m_hidePages: List[_RibbonTabData] = list()

        self.m_applicationButton = RibbonApplicationButton(self)
        self.m_applicationButton.clicked.connect(self.applicationButtonClicked)
        self.m_tabBar = RibbonTabBar(self)
        self.m_tabBar.setObjectName("qx_RibbonTabBar")
        self.m_tabBar.setDrawBase(False)
        self.m_tabBar.currentChanged.connect(self.onCurrentRibbonTabChanged)
        self.m_tabBar.tabBarClicked.connect(self.onCurrentRibbonTabClicked)
        self.m_tabBar.tabBarDoubleClicked.connect(self.onCurrentRibbonTabDoubleClicked)
        self.m_tabBar.tabMoved.connect(self.onTabMoved)

        self.m_stack = RibbonStackedWidget(self)
        self.m_stack.setObjectName("qx_RibbonStackedWidget")
        self.m_stack.aboutToHide.connect(self.onStackWidgetHided)
        self.m_stack.installEventFilter(self)
        self._setMinimizedFlag(False)

        self.m_quickAccessBar = RibbonQuickAccessBar(self)
        self.m_quickAccessBar.setObjectName("qx_RibbonQuickAccessBar")
        self.m_quickAccessBar.setIcon(self.windowIcon())

        lineSpacing = QApplication.fontMetrics().lineSpacing()
        self.m_titleBarHeight = lineSpacing * 1.8
        self.m_tabBarHeight = lineSpacing * 1.5
        self.m_windowButtonsSize = QSize(int(self.m_titleBarHeight * 4), int(self.m_titleBarHeight))
        self.m_ribbonBarHeightOfficeStyleThreeRow = 0
        self.m_ribbonBarHeightWpsLiteStyleThreeRow = 0
        self.m_ribbonBarHeightOfficeStyleTwoRow = 0
        self.m_ribbonBarHeightWpsLiteStyleTwoRow = 0

        self.recalc()

        if parent:
            parent.windowTitleChanged.connect(self.onWindowTitleChanged)
            parent.windowIconChanged.connect(self.onWindowIconChanged)
        self.setRibbonStyle(RibbonBar.OfficeStyle)

    def applicationButton(self) -> QAbstractButton:
        return self.m_applicationButton

    def setApplicationButton(self, btn: QAbstractButton):
        if not btn:
            return
        if btn.parent() != self:
            btn.setParent(self)
        btn.move(0, self.titleBarHeight())
        self.m_applicationButton = btn
        self.m_applicationButton.clicked.connect(self.applitionButtonClicked)
        if not btn.objectName():
            btn.setObjectName('RibbonApplicationButton')
        btn.setVisible(True)
        QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))

    def ribbonTabBar(self) -> RibbonTabBar:
        return self.m_tabBar

    def addPage(self, *args) -> Union[None, RibbonPage]:
        """
        addPage(title: str) -> RibbonPage
        addPage(page: RibbonPage) -> None
        """
        if len(args) == 1:
            if isinstance(args[0], str):
                return self.insertPage(-1, args[0])
            else:
                self.insertPage(-1, args[0])

    def _insertPage(self, index: int, page: RibbonPage):
        page.setGroupLayoutMode(RibbonGroup.TwoRowMode if self.isTwoRowStyle() else
                                RibbonGroup.ThreeRowMode)
        i = self.m_tabBar.insertTab(index, page.windowTitle())
        tabData = _RibbonTabData()
        tabData.page = page
        tabData.index = i
        self.m_tabBar.setTabData(i, tabData)
        if index != -1:
            self._updateTabData()
        self.m_stack.insertWidget(i, page)
        page.windowTitleChanged.connect(self.onPageWindowTitleChanged)
        QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))

    def insertPage(self, *args) -> Union[None, RibbonPage]:
        """
        insertPage(index: int, title: str) -> RibbonPage
        insertPage(index: int, page: RibbonPage) -> None
        """
        if len(args) == 2:
            if isinstance(args[1], str):
                title = args[1]
                page = RibbonPage(self)
                page.setObjectName(title)
                page.setWindowTitle(title)
                self._insertPage(args[0], page)
                return page
            elif isinstance(args[1], RibbonPage):
                self._insertPage(args[0], args[1])

    def page(self, index: int) -> RibbonPage:
        var = self.m_tabBar.tabData(index)
        if var:
            return var.page
        return None

    def pageByName(self, title: str) -> RibbonPage:
        c = self.m_stack.count()
        for i in range(c):
            w = self.m_stack.widget(i)
            if w and w.windowTitle() == title:
                return w
        return None

    def pageByObjectName(self, objname: str) -> RibbonPage:
        c = self.m_stack.count()
        for i in range(c):
            w = self.m_stack.widget(i)
            if w and w.objectName() == objname:
                return w
        return None

    def pages(self, get_all: bool = True) -> List[RibbonPage]:
        c = self.m_stack.count()
        res = []
        for i in range(c):
            w = self.m_stack.widget(i)
            if isinstance(w, RibbonPage):
                page: RibbonPage = w
                if not get_all and page.isPageContext():
                    continue
                res.append(page)
        return res

    def isPageVisible(self, page: RibbonPage) -> bool:
        return self.pageIndex(page) >= 0

    def showPage(self, page: RibbonPage):
        for i, data in enumerate(self.m_hidePages):
            if page == data.page:
                index = self.m_tabBar.insertTab(data.index, data.page.windowTitle())
                data.index = index
                self.m_tabBar.setTabData(index, data)
                self.m_hidePages.pop(i)
                self._updateTabData()
                break
        self.raisePage(page)

    def hidePage(self, page: RibbonPage):
        c = self.m_tabBar.count()
        for i in range(c):
            var: _RibbonTabData = self.m_tabBar.tabData(i)
            if var.page == page:
                self.m_hidePages.append(var)
                self.m_tabBar.removeTab(i)
                self._updateTabData()
                return

    def raisePage(self, page: RibbonPage):
        index = self.m_stack.indexOf(page)
        if index >= 0:
            self.setCurrentIndex(index)

    def removePage(self, page: RibbonPage):
        index = self._tabIndex(page)
        isUpdate = False
        if index >= 0:
            self.m_tabBar.removeTab(index)
            isUpdate = True
        self.m_stack.removeWidget(page)
        for c in self.m_pageContextList:
            c.takePage(page)
        if isUpdate:
            self._updateTabData()
        self.repaint()
        QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))

    def movePage(self, fr: int, to: int):
        self.m_tabBar.moveTab(fr, to)
        self._updateTabData()

    def pageIndex(self, page: RibbonPage) -> int:
        return self._tabIndex(page)

    def _addPageContext(self, context: RibbonPageContext):
        context.pageAdded.connect(self.onPageContextAdded)
        if self.currentRibbonStyle() == RibbonBar.WpsLiteStyle:
            self._resizeInWpsLiteStyle()
        self.m_pageContextList.append(context)

    def addPageContext(self, *args) -> Union[None, RibbonPageContext]:
        """
        addPageContext(title: str, color = QColor(), id = QVariant()) -> RibbonPageContext
        addPageContext(context: RibbonPageContext) -> None
        """
        arg_len = len(args)
        if arg_len > 0:
            if isinstance(args[0], RibbonPageContext):
                self._addPageContext(args[0])
            else:
                title = args[0]
                color = args[1] if arg_len > 1 else QColor()
                contextId = args[2] if arg_len > 2 else QVariant()
                context = RibbonPageContext(self)
                context.setObjectName(title)
                context.setContextTitle(title)
                context.setId(contextId)
                context.setContextColor(color if color.isValid() else self._getPageContextColor())
                self._addPageContext(context)
                return context

    def showPageContext(self, context: RibbonPageContext):
        if self.isPageContextVisible(context):
            return
        pageContextData = _PageContextManagerData()
        pageContextData.pageContext = context
        for i in range(context.pageCount()):
            page = context.page(i)
            page.setGroupLayoutMode(RibbonGroup.TwoRowMode if self.isTwoRowStyle() else
                                    RibbonGroup.ThreeRowMode)
            index = self.m_tabBar.addTab(page.windowTitle())
            pageContextData.tabPageIndex.append(index)

            tabData = _RibbonTabData()
            tabData.page = page
            tabData.index = index
            self.m_tabBar.setTabData(index, tabData)
        self.m_currentShowingPageContextList.append(pageContextData)
        QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))

    def hidePageContext(self, context: RibbonPageContext):
        needResize = False
        indexOffset = 0
        for i, data in enumerate(self.m_currentShowingPageContextList):
            if data.compare(context):
                indexes = data.tabPageIndex
                for index in reversed(indexes):
                    self.m_tabBar.removeTab(index)
                    indexOffset += 1
                needResize = True
                self.m_currentShowingPageContextList.pop(i)
                break
        if needResize:
            self._updateTabData()
            QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))

    def isPageContextVisible(self, context: RibbonPageContext) -> bool:
        return self._isContainPageContextInList(context)

    def setPageContextVisible(self, context: RibbonPageContext, visible: bool):
        if not context:
            return
        if visible:
            self.showPageContext(context)
        else:
            self.hidePageContext(context)

    def pageContextList(self) -> List[RibbonPageContext]:
        return self.m_pageContextList

    def destroyPageContext(self, context: RibbonPageContext):
        if self.isPageContextVisible(context):
            self.hidePageContext(context)
        self.m_pageContextList.remove(context)
        res = context.pageList()
        for page in res:
            page.hide()
            # page.deleteLater()
        context.deleteLater()
        QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))

    def minimize(self):
        self.setMinimized(True)

    def isMinimized(self) -> bool:
        return self.m_minimized

    def setMinimized(self, flag: bool):
        self._setMinimizedFlag(flag)
        QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))
        self.minimizationChanged.emit(flag)

    def haveShowMinimumButton(self) -> bool:
        return self.m_minimumPageButton is not None

    def showMinimumButton(self, shown: bool = True):
        if shown:
            self.activeRightButtonGroup()
            if not self.m_minimumPageButton:
                self.m_minimumPageButton = RibbonControlButton(self)
                self.m_minimumPageButton.setAutoRaise(False)
                self.m_minimumPageButton.setObjectName('RibbonBarHideGroupButton')
                self.m_minimumPageButton.setToolButtonStyle(Qt.ToolButtonIconOnly)
                self.m_minimumPageButton.setFixedSize(self.tabBarHeight() - 4, self.tabBarHeight() - 4)
                self.m_minimumPageButton.ensurePolished()
                action = QAction(self.m_minimumPageButton)
                action.setIcon(self.style().standardIcon(QStyle.SP_TitleBarUnshadeButton if self.isMinimized() else
                                                         QStyle.SP_TitleBarShadeButton, None))
                action.triggered.connect(lambda: (
                    self.setMinimized(not self.isMinimized()),
                    action.setIcon(self.style().standardIcon(QStyle.SP_TitleBarUnshadeButton if self.isMinimized() else
                                                             QStyle.SP_TitleBarShadeButton, None))
                ))
                self.m_minimumPageButton.setDefaultAction(action)
                self.m_rightButtonGroup.addWidget(self.m_minimumPageButton)
                self.update()
            else:
                self.m_minimumPageButton.show()
        else:
            if self.m_minimumPageButton:
                self.m_minimumPageButton.hide()
        QApplication.sendEvent(self, QResizeEvent(self.size(), self.size()))

    def tabBarHeight(self) -> int:
        return round(self.m_tabBarHeight)

    def titleBarHeight(self) -> int:
        return round(self.m_titleBarHeight)

    def rightButtonGroup(self) -> RibbonButtonGroup:
        self.activeRightButtonGroup()
        return self.m_rightButtonGroup

    def activeRightButtonGroup(self):
        if not self.m_rightButtonGroup:
            self.m_rightButtonGroup = RibbonButtonGroup(self)
            self.m_rightButtonGroup.setFrameShape(QFrame.NoFrame)
        self.m_rightButtonGroup.show()

    def quickAccessBar(self) -> RibbonQuickAccessBar:
        return self.m_quickAccessBar

    def currentRibbonStyle(self) -> int:
        return self.m_ribbonStyle

    def setRibbonStyle(self, style: int):
        self.m_ribbonStyle = style
        self.m_lastShowStyle = style
        self.m_quickAccessBar.setIconVisible(self.isOfficeStyle())
        self._updateRibbonElementGeometry()

        oldSize = self.size()
        newSize = QSize(oldSize.width(), self._mainBarHeight())
        QApplication.sendEvent(self, QResizeEvent(newSize, oldSize))
        if self.isMinimized():
            self.setFixedHeight(self.m_tabBar.geometry().bottom())
        self.ribbonStyleChanged.emit(style)

    def currentIndex(self) -> int:
        return self.m_tabBar.currentIndex()

    def setCurrentIndex(self, index: int):
        return self.m_tabBar.setCurrentIndex(index)

    def isOfficeStyle(self) -> bool:
        return (self.m_ribbonStyle & 0x00FF) == 0

    def isTwoRowStyle(self) -> bool:
        return (self.m_ribbonStyle & 0xFF00) != 0

    def setWindowButtonsSize(self, size: QSize):
        self.m_windowButtonsSize = size

    def updateRibbonGeometry(self):
        self._updateRibbonElementGeometry()
        for page in self.pages():
            page.updateItemGeometry()

    def tabBarBaseLineColor(self) -> QColor:
        return self.m_tabBarBaseLineColor

    def setTabBarBaseLineColor(self, color: QColor):
        self.m_tabBarBaseLineColor = color

    def windowTitleTextColor(self) -> QColor:
        return self.m_titleTextColor

    def setWindowTitleTextColor(self, color: QColor):
        self.m_titleTextColor = color

    def windowTitleAlignment(self) -> int:
        return self.m_titleAlignment

    def setWindowTitleAlignment(self, align: int):
        self.m_titleAlignment = align

    def eventFilter(self, obj: QObject, event: QEvent) -> bool:
        if not obj:
            return super().eventFilter(obj, event)

        if obj == self.cornerWidget(Qt.TopLeftCorner) or obj == self.cornerWidget(Qt.TopRightCorner):
            if event.type() in (QEvent.UpdateLater, QEvent.MouseButtonRelease, QEvent.WindowActivate):
                QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))
        elif obj == self.m_stack:
            '''
            在stack 是popup模式时，点击的是stackWidget区域外的时候，如果是在ribbonTabBar上点击
            那么忽略掉这次点击，把点击下发到ribbonTabBar,这样可以避免stackWidget在点击ribbonTabBar后
            隐藏又显示，出现闪烁
            '''
            if event.type() in (QEvent.MouseButtonPress, QEvent.MouseButtonDblClick) and self.m_stack.isPopup():
                mouseEvent: QMouseEvent = event
                if not self.m_stack.rect().contains(mouseEvent.pos()):
                    clickedWidget = QApplication.widgetAt(mouseEvent.globalPos())
                    if clickedWidget == self.m_tabBar:
                        targetPoint = clickedWidget.mapFromGlobal(mouseEvent.globalPos())
                        ePress = QMouseEvent(mouseEvent.type(), targetPoint, mouseEvent.globalPos(),
                                             mouseEvent.button(), mouseEvent.buttons(),
                                             mouseEvent.modifiers())
                        QApplication.postEvent(clickedWidget, ePress)
                        return True
        return super().eventFilter(obj, event)

    def paintEvent(self, event: QPaintEvent):
        p = QPainter(self)
        if self.isOfficeStyle():
            self._paintInOfficeStyle(p)
        else:
            self._paintInWpsLiteStyle(p)

    def resizeEvent(self, event: QResizeEvent):
        if self.isOfficeStyle():
            self._resizeInOfficeStyle()
        else:
            self._resizeInWpsLiteStyle()
        self.update()

    def moveEvent(self, event: QMoveEvent):
        if self.m_stack:
            if self.m_stack.isPopup():
                self._resizeStackedWidget()
        super().moveEvent(event)

    def onWindowTitleChanged(self, title: str):
        self.update()

    def onWindowIconChanged(self, icon: QIcon):
        if self.m_quickAccessBar:
            self.m_quickAccessBar.setIcon(icon)

    def onPageWindowTitleChanged(self, title: str):
        w = self.sender()
        if not w:
            return
        for i in range(self.m_tabBar.count()):
            p = self.m_tabBar.tabData(i)
            if w == p.page:
                self.m_tabBar.setTabText(i, title)

    def onPageContextAdded(self, page: RibbonPage):
        self.m_stack.addWidget(page)

    def onCurrentRibbonTabChanged(self, index: int):
        tabData: _RibbonTabData = self.m_tabBar.tabData(index)
        if tabData and tabData.page:
            page = tabData.page
            if self.m_stack.currentWidget() != page:
                self.m_stack.setCurrentWidget(page)
            if self.isMinimized():
                self.m_quickAccessBar.clearFocus()
                if not self.m_stack.isVisible() and self.m_stack.isPopup():
                    # 在stackWidget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
                    ehl = QHoverEvent(QEvent.HoverLeave, self.m_tabBar.mapToGlobal(QCursor.pos()),
                                      self.m_tabBar.mapToGlobal(QCursor.pos()))
                    QApplication.sendEvent(self.m_tabBar, ehl)
                    self._resizeStackedWidget()
                    self.m_stack.setFocus()
                    self.m_stack.show()
        self.currentRibbonTabChanged.emit(index)

    def onCurrentRibbonTabClicked(self, index: int):
        if index != self.m_tabBar.currentIndex():
            return
        if self.isMinimized():
            if not self.m_stack.isVisible() and self.m_stack.isPopup():
                # 在stackWidget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
                ehl = QHoverEvent(QEvent.HoverLeave, self.m_tabBar.mapToGlobal(QCursor.pos()),
                                  self.m_tabBar.mapToGlobal(QCursor.pos()))
                QApplication.sendEvent(self.m_tabBar, ehl)
                self._resizeStackedWidget()
                self.m_stack.setFocus()
                self.m_stack.show()

    def onCurrentRibbonTabDoubleClicked(self, index: int):
        self.setMinimized(not self.isMinimized())

    def onTabMoved(self, fr: int, to: int):
        self.m_stack.moveWidget(fr, to)

    def onStackWidgetHided(self):
        pass

    def recalc(self):
        self.m_ribbonBarHeightOfficeStyleThreeRow = self._calcRibbonBarHeight(RibbonBar.OfficeStyle)
        self.m_ribbonBarHeightWpsLiteStyleThreeRow = self._calcRibbonBarHeight(RibbonBar.WpsLiteStyle)
        self.m_ribbonBarHeightOfficeStyleTwoRow = self._calcRibbonBarHeight(RibbonBar.OfficeStyleTwoRow)
        self.m_ribbonBarHeightWpsLiteStyleTwoRow = self._calcRibbonBarHeight(RibbonBar.WpsLiteStyleTwoRow)

    def _calcRibbonBarHeight(self, style) -> int:
        if style == RibbonBar.OfficeStyle:
            return self.titleBarHeight() + self.tabBarHeight() + \
                   QApplication.fontMetrics().lineSpacing() * 1.5 * 3 + \
                   RibbonGroupLayout.groupContentsMargins().top() + \
                   RibbonGroupLayout.groupContentsMargins().bottom() + \
                   RibbonGroup.groupTitleHeight()
        elif style == RibbonBar.WpsLiteStyle:
            return self.m_ribbonBarHeightOfficeStyleThreeRow - self.tabBarHeight()
        elif style == RibbonBar.WpsLiteStyleTwoRow:
            return int(self.m_ribbonBarHeightOfficeStyleThreeRow * 0.95 - self.tabBarHeight() -
                       RibbonGroup.groupTitleHeight())
        elif style == RibbonBar.OfficeStyleTwoRow:
            return int(self.m_ribbonBarHeightOfficeStyleThreeRow * 0.95 - RibbonGroup.groupTitleHeight())
        else:
            return self.m_ribbonBarHeightOfficeStyleThreeRow

    def _isContainPageContextInList(self, context: RibbonPageContext) -> bool:
        if not context:
            return False
        for data in self.m_currentShowingPageContextList:
            if data.compare(context):
                return True
        return False

    def _setMinimizedFlag(self, flag: bool):
        if self.m_minimized == flag:
            return
        self.m_minimized = flag
        if flag:
            self.m_stack.setPopup(True)
            self.m_stack.setFocusPolicy(Qt.NoFocus)
            self.m_stack.clearFocus()
            self.m_stack.setFocus()
            self.m_stack.hide()
            self.setFixedHeight(self.m_tabBar.geometry().bottom())
        else:
            self.m_stack.setPopup(False)
            self.m_stack.setFocus()
            self.m_stack.show()

    def _getPageContextColor(self) -> QColor:
        if not self.m_pageContextColorList:
            self.m_pageContextColorListIndex = -1
            return QColor()
        self.m_pageContextColorListIndex += 1
        if self.m_pageContextColorListIndex >= len(self.m_pageContextColorList) or \
                self.m_pageContextColorListIndex < 0:
            self.m_pageContextColorListIndex = 0
        return self.m_pageContextColorList[self.m_pageContextColorListIndex]

    def _updateTabData(self):
        c = self.m_tabBar.count()
        for i in range(c):
            data: _RibbonTabData = self.m_tabBar.tabData(i)
            if data:
                data.index = i
                self.m_tabBar.setTabData(i, data)
        self._updatePageContextManagerData()

    def _updatePageContextManagerData(self):
        for cd in self.m_currentShowingPageContextList:
            cd.tabPageIndex.clear()
            for i in range(cd.pageContext.pageCount()):
                page = cd.pageContext.page(i)
                for j in range(self.m_tabBar.count()):
                    data: _RibbonTabData = self.m_tabBar.tabData(j)
                    if data:
                        if data.page == page:
                            cd.tabPageIndex.append(j)
                    else:
                        cd.tabPageIndex.append(-1)

    def _updateRibbonElementGeometry(self):
        for page in self.pages():
            page.setGroupLayoutMode(RibbonGroup.TwoRowMode if self.isTwoRowStyle() else
                                    RibbonGroup.ThreeRowMode)
        if not self.isMinimized():
            self.setFixedHeight(int(self._mainBarHeight()))

    def _paintInOfficeStyle(self, p: QPainter):
        self._paintBackground(p)
        p.save()
        pageContextPos = QPoint(self.width(), -1)
        border = self.contentsMargins()
        for cd in self.m_currentShowingPageContextList:
            indexes = cd.tabPageIndex
            clr = cd.pageContext.contextColor()
            if indexes:
                contextTitleRect = QRect(self.m_tabBar.tabRect(indexes[0]))
                endRect = QRect(self.m_tabBar.tabRect(indexes[-1]))
                contextTitleRect.setRight(endRect.right())
                contextTitleRect.translate(self.m_tabBar.x(), self.m_tabBar.y())
                contextTitleRect.setHeight(self.m_tabBar.height() - 1)
                contextTitleRect -= self.m_tabBar.tabMargin()
                contextTitleRect.setTop(border.top())
                self._paintPageContextTab(p, cd.pageContext.contextTitle(), contextTitleRect, clr)
                if contextTitleRect.left() < pageContextPos.x():
                    pageContextPos.setX(contextTitleRect.left())
                if contextTitleRect.right() > pageContextPos.y():
                    pageContextPos.setY(contextTitleRect.right())

            if self.m_tabBar.currentIndex() in indexes:
                pen = QPen()
                pen.setColor(clr)
                pen.setWidth(1)
                p.setPen(pen)
                p.setBrush(Qt.NoBrush)
                p.drawRect(self.m_stack.geometry())
        p.restore()
        parWindow = self.parentWidget()
        if parWindow:
            titleRegion = QRect()
            if pageContextPos.y() < 0:
                titleRegion.setRect(self.m_quickAccessBar.geometry().right() + 1, border.top(),
                                    self.width() - self.m_iconRightBorderPosition - border.right() -
                                    self.m_windowButtonsSize.width() - self.m_quickAccessBar.geometry().right() - 1,
                                    int(self.titleBarHeight()))
            else:
                leftWidth = pageContextPos.x() - self.m_quickAccessBar.geometry().right() - \
                            self.m_iconRightBorderPosition
                rightWidth = self.width() - pageContextPos.y() - self.m_windowButtonsSize.width()
                if rightWidth > leftWidth:
                    titleRegion.setRect(pageContextPos.y(), border.top(), rightWidth, self.titleBarHeight())
                else:
                    titleRegion.setRect(self.m_iconRightBorderPosition + self.m_quickAccessBar.geometry().right(),
                                        border.top(), leftWidth, self.titleBarHeight())
            self._paintWindowTitle(p, parWindow.windowTitle(), titleRegion)

    def _paintInWpsLiteStyle(self, p: QPainter):
        self._paintBackground(p)
        p.save()
        border = self.contentsMargins()
        for cd in self.m_currentShowingPageContextList:
            indexes = cd.tabPageIndex
            clr = cd.pageContext.contextColor()
            if indexes:
                contextTitleRect = QRect(self.m_tabBar.tabRect(indexes[0]))
                endRect = QRect(self.m_tabBar.tabRect(indexes[-1]))
                contextTitleRect.setRight(endRect.right())
                contextTitleRect.translate(self.m_tabBar.x(), self.m_tabBar.y())
                contextTitleRect.setHeight(self.m_tabBar.height() - 1)
                contextTitleRect -= self.m_tabBar.tabMargin()
                contextTitleRect.setTop(border.top())
                self._paintPageContextTab(p, "", contextTitleRect, clr)
            if self.m_tabBar.currentIndex() in indexes:
                pen = QPen()
                pen.setColor(clr)
                pen.setWidth(1)
                p.setPen(pen)
                p.setBrush(Qt.NoBrush)
                p.drawRect(self.m_stack.geometry())
        p.restore()
        parWindow = self.parentWidget()
        if parWindow:
            start = self.m_tabBar.x() + self.m_tabBar.width()
            width = self.m_quickAccessBar.x() - start
            if width > 20:
                titleRegion = QRect(start, border.top(), width, self.titleBarHeight())
                self._paintWindowTitle(p, parWindow.windowTitle(), titleRegion)

    def _paintBackground(self, painter: QPainter):
        painter.save()
        lineY = self.m_tabBar.geometry().bottom()
        pen = QPen(self.m_tabBarBaseLineColor)
        border = self.contentsMargins()
        pen.setWidth(1)
        pen.setStyle(Qt.SolidLine)
        painter.setPen(pen)
        painter.drawLine(QPoint(border.left(), lineY),
                         QPoint(self.width() - border.right() - 1, lineY))
        painter.restore()

    def _paintWindowTitle(self, painter: QPainter, title: str, rect: QRect):
        painter.save()
        painter.setPen(self.m_titleTextColor)
        painter.drawText(rect, self.m_titleAlignment, title)
        painter.restore()

    def _paintPageContextTab(self, painter: QPainter, title: str, rect: QRect, color: QColor):
        border = self.contentsMargins()
        painter.save()
        painter.setPen(Qt.NoPen)
        painter.setBrush(color)
        painter.drawRect(QRect(rect.x(), border.top(), rect.width(), 5))

        gColor = color.lighter(190)
        newContextRect = QRect(rect)
        newContextRect -= QMargins(0, 5, 0, 0)
        painter.fillRect(newContextRect, gColor)
        if self.isOfficeStyle() and title:
            newContextRect.setBottom(self.m_tabBar.geometry().top())
            painter.setPen(color)
            painter.drawText(newContextRect, Qt.AlignCenter, title)
        painter.restore()

    def _resizeInOfficeStyle(self):
        self._updateRibbonElementGeometry()
        border = self.contentsMargins()
        x = border.left()
        y = border.top()

        validTitleBarHeight = self.titleBarHeight()
        tabH = self.tabBarHeight()
        x += self.m_iconRightBorderPosition + 5

        connerL = self.cornerWidget(Qt.TopLeftCorner)
        if connerL and connerL.isVisible():
            connerSize = connerL.sizeHint()
            if connerSize.height() < validTitleBarHeight:
                detal = (validTitleBarHeight - connerSize.height()) / 2
                connerL.setGeometry(x, y + detal, connerSize.width(), connerSize.height())
            else:
                connerL.setGeometry(x, y, connerSize.width(), validTitleBarHeight)
            x = connerL.geometry().right() + 5
        # quick access bar定位
        if self.m_quickAccessBar and self.m_quickAccessBar.isVisible():
            if self.m_quickAccessBar.height() != validTitleBarHeight:
                self.m_quickAccessBar.setFixedHeight(int(validTitleBarHeight))
            quickAccessBarSize = self.m_quickAccessBar.sizeHint()
            self.m_quickAccessBar.setGeometry(x, y, quickAccessBarSize.width(), int(validTitleBarHeight))
        # 第二行，开始布局applitionButton，tabbar，tabBarRightSizeButtonGroupWidget，TopRightCorner
        x = border.left()
        y += validTitleBarHeight
        if self.m_applicationButton and self.m_applicationButton.isVisible():
            self.m_applicationButton.setGeometry(x, int(y), self.m_applicationButton.size().width(), int(tabH))
            x = self.m_applicationButton.geometry().right()
        # top right是一定要配置的，对于多文档窗口，子窗口的缩放等按钮就是通过这个窗口实现
        endX = self.width() - border.right()
        connerR = self.cornerWidget(Qt.TopRightCorner)
        if connerR and connerR.isVisible():
            connerSize = connerR.sizeHint()
            endX -= connerSize.width()
            if connerSize.height() < tabH:
                detal = (tabH - connerSize.height()) / 2
                connerR.setGeometry(endX, y + detal, connerSize.width(), connerSize.height())
            else:
                connerR.setGeometry(endX, y, connerSize.width(), tabH)
        # tabBar 右边的附加按钮组，这里一般会附加一些类似登录等按钮组
        if self.m_rightButtonGroup and self.m_rightButtonGroup.isVisible():
            wSize = self.m_rightButtonGroup.sizeHint()
            endX -= wSize.width()
            self.m_rightButtonGroup.setGeometry(endX, y, wSize.width(), tabH)
        # 最后确定tabbar宽度
        tabBarWidth = endX - x
        self.m_tabBar.setGeometry(int(x), int(y), tabBarWidth, int(tabH))
        # 调整整个stackWidget
        self._resizeStackedWidget()

    def _resizeInWpsLiteStyle(self):
        self._updateRibbonElementGeometry()
        border = self.contentsMargins()
        x = border.left()
        y = border.top()

        validTitleBarHeight = self.titleBarHeight()
        # 先布局右边内容
        # cornerWidget - TopRightCorner
        endX = self.width() - border.right() - self.m_windowButtonsSize.width()
        connerR = self.cornerWidget(Qt.TopRightCorner)
        if connerR and connerR.isVisible():
            connerSize = connerR.sizeHint()
            endX -= connerSize.width()
            if connerSize.height() < validTitleBarHeight:
                detal = (validTitleBarHeight - connerSize.height()) / 2
                connerR.setGeometry(endX, y + detal, connerSize.width(), connerSize.height())
            else:
                connerR.setGeometry(endX, y, connerSize.width(), int(validTitleBarHeight))
        # tabBar 右边的附加按钮组
        if self.m_rightButtonGroup and self.m_rightButtonGroup.isVisible():
            wSize = self.m_rightButtonGroup.sizeHint()
            endX -= wSize.width()
            self.m_rightButtonGroup.setGeometry(endX, y, wSize.width(), int(validTitleBarHeight))
        # quick access bar定位
        if self.m_quickAccessBar and self.m_quickAccessBar.isVisible():
            quickAccessBarSize = self.m_quickAccessBar.sizeHint()
            endX -= quickAccessBarSize.width()
            self.m_quickAccessBar.setGeometry(endX, y, quickAccessBarSize.width(), int(validTitleBarHeight))
        # cornerWidget - TopLeftCorner
        connerL = self.cornerWidget(Qt.TopLeftCorner)
        if connerR and connerL.isVisible():
            connerSize = connerR.sizeHint()
            endX -= connerSize.width()
            if connerSize.height() < validTitleBarHeight:
                detal = (validTitleBarHeight - connerSize.height()) / 2
                connerL.setGeometry(endX, y + detal, connerSize.width(), connerSize.height())
            else:
                connerL.setGeometry(endX, y, connerSize.width(), int(validTitleBarHeight))
        # tab 的y值需要重新计算
        tabH = self.tabBarHeight()
        if tabH > validTitleBarHeight:
            tabH = validTitleBarHeight
        y += validTitleBarHeight - tabH

        if self.m_applicationButton and self.m_applicationButton.isVisible():
            self.m_applicationButton.setGeometry(x, y, self.m_applicationButton.size().width(), tabH)
            x = self.m_applicationButton.geometry().right() + 2

        tabBarWidth = endX - x
        mintabBarWidth = self._calcMinTabBarWidth()
        if mintabBarWidth < tabBarWidth:
            tabBarWidth = mintabBarWidth
        self.m_tabBar.setGeometry(x, y, tabBarWidth, tabH)
        # 调整整个stackWidget
        self._resizeStackedWidget()

    def _resizeStackedWidget(self):
        border = self.contentsMargins()
        if self.m_stack.isPopup():
            absPosition = self.mapToGlobal(QPoint(border.left(), self.m_tabBar.geometry().bottom() + 1))
            self.m_stack.setGeometry(
                absPosition.x(), absPosition.y(),
                self.width() - border.left() - border.right(),
                self._mainBarHeight() - self.m_tabBar.geometry().bottom() - border.bottom() - 1
            )
        else:
            self.m_stack.setGeometry(
                border.left(), self.m_tabBar.geometry().bottom() + 1,
                self.width() - border.left() - border.right(),
                round(self._mainBarHeight() - self.m_tabBar.geometry().bottom() - border.bottom() - 1)
            )

    def _calcMinTabBarWidth(self) -> int:
        m = self.m_tabBar.tabMargin()
        mintabBarWidth = 0
        for i in range(self.m_tabBar.count()):
            mintabBarWidth += self.m_tabBar.tabRect(i).width()
        mintabBarWidth += m.left() + m.right()
        return round(mintabBarWidth)

    def _ribbonBarHeight(self, style) -> int:
        if style == RibbonBar.OfficeStyle:
            return self.m_ribbonBarHeightOfficeStyleThreeRow
        elif style == RibbonBar.WpsLiteStyle:
            return self.m_ribbonBarHeightWpsLiteStyleThreeRow
        elif style == RibbonBar.OfficeStyleTwoRow:
            return self.m_ribbonBarHeightOfficeStyleTwoRow
        elif style == RibbonBar.WpsLiteStyleTwoRow:
            return self.m_ribbonBarHeightWpsLiteStyleTwoRow
        else:
            return self.m_ribbonBarHeightOfficeStyleThreeRow

    def _mainBarHeight(self) -> int:
        return round(self._ribbonBarHeight(self.currentRibbonStyle()))

    def _tabIndex(self, page: RibbonPage) -> int:
        c = self.m_tabBar.count()
        for i in range(c):
            var = self.m_tabBar.tabData(i)
            if var.page == page:
                return i
        return -1

    # RibbonStyle
    OfficeStyle = 0x0000
    WpsLiteStyle = 0x0001
    OfficeStyleTwoRow = 0x0100 | OfficeStyle
    WpsLiteStyleTwoRow = 0x0100 | WpsLiteStyle

    # signals
    minimizationChanged = pyqtSignal(bool)
    applicationButtonClicked = pyqtSignal()
    currentRibbonTabChanged = pyqtSignal(int)
    ribbonStyleChanged = pyqtSignal(int)
