#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

from typing import List, Union, Dict

from PyQt5.QtCore import QSize, Qt, pyqtSignal, QVariant, QPoint, \
    QAbstractListModel, QModelIndex, QItemSelectionModel
from PyQt5.QtGui import QPalette, QIcon, QPainter, QPaintEvent, QResizeEvent
from PyQt5.QtWidgets import QWidget, QFrame, QAction, QActionGroup, QLabel, QStyleOptionViewItem, \
    QStyledItemDelegate, QListView, QStyle, QVBoxLayout, QSizePolicy, QBoxLayout, QApplication

from .QxRibbonControls import RibbonControlButton
from . import QxRibbonRes_rc


class RibbonGalleryItem:
    def __init__(self, *args):
        """
        RibbonGalleryItem()
        RibbonGalleryItem(text: str, icon: QIcon)
        RibbonGalleryItem(action: QAction)
        """
        self.m_datas: Dict[int, QVariant] = {}
        self.m_flags = Qt.ItemIsEnabled | Qt.ItemIsSelectable
        self.m_action: QAction = None
        arg_len = len(args)
        if arg_len == 2:
            self.setText(args[0])
            self.setIcon(args[1])
            self.setTextAlignment(Qt.AlignTop | Qt.AlignHCenter)
        elif arg_len == 1:
            self.setTextAlignment(Qt.AlignTop | Qt.AlignHCenter)
            self.setAction(args[0])

    def setData(self, role: int, data: QVariant):
        self.m_datas[role] = QVariant(data)

    def data(self, role: int) -> QVariant:
        if self.m_action:
            if role == Qt.DisplayRole:
                return self.m_action.text()
            elif role == Qt.ToolTipRole:
                return self.m_action.toolTip()
            elif role == Qt.DecorationRole:
                return self.m_action.icon()
        return self.m_datas.get(role, None)

    def setText(self, text: str):
        self.setData(Qt.DisplayRole, text)

    def text(self) -> str:
        if self.m_action:
            return self.m_action.text()
        return str(self.data(Qt.DisplayRole).value())

    def setToolTip(self, text: str):
        self.setData(Qt.ToolTipRole, text)

    def toolTip(self) -> str:
        if self.m_action:
            return self.m_action.tooltip()
        return str(self.data(Qt.ToolTipRole).value())

    def setIcon(self, icon: QIcon):
        self.setData(Qt.DecorationRole, icon)

    def icon(self) -> QIcon:
        if self.m_action:
            return self.m_action.tooltip()
        return QIcon(self.data(Qt.ToolTipRole).value())

    def isSelectable(self) -> bool:
        return self.m_flags & Qt.ItemIsSelectable

    def setSelectable(self, selectable: bool):
        if selectable:
            self.m_flags |= Qt.ItemIsSelectable
        else:
            self.m_flags &= ~Qt.ItemIsSelectable

    def isEnable(self) -> bool:
        if self.m_action:
            return self.m_action.isEnabled()
        return self.m_flags & Qt.ItemIsEnabled

    def setEnable(self, enable: bool):
        if self.m_action:
            self.m_action.setEnabled(enable)
        if enable:
            self.m_flags |= Qt.ItemIsEnabled
        else:
            self.m_flags &= ~Qt.ItemIsEnabled

    def setFlags(self, flag):
        self.m_flags = flag
        if self.m_action:
            self.m_action.setEnabled(flag & Qt.ItemIsEnabled)

    def flags(self) -> int:
        return self.m_flags

    def setAction(self, action: QAction):
        self.m_action = action
        if not action:
            return
        if action.isEnabled():
            self.m_flags |= Qt.ItemIsEnabled
        else:
            self.m_flags &= ~Qt.ItemIsEnabled

    def action(self) -> QAction:
        return self.m_action

    def setTextAlignment(self, align):
        self.setData(Qt.TextAlignmentRole, int(align))

    def getTextAlignment(self) -> int:
        return int(self.data(Qt.TextAlignmentRole).value())


class RibbonGalleryGroupItemDelegate(QStyledItemDelegate):
    def __init__(self, group, parent=None):
        assert group
        super().__init__(parent)
        self.m_group = group

    def paint(self, painter: QPainter, option: QStyleOptionViewItem, index: QModelIndex):
        t = self.m_group.getGalleryGroupStyle()
        if t == RibbonGalleryGroup.IconWidthText:
            self.paintIconWithText(painter, option, index)
        elif t == RibbonGalleryGroup.IconWithWordWrapText:
            self.paintIconWithTextWordWrap(painter, option, index)
        elif t == RibbonGalleryGroup.IconOnly:
            self.paintIconOnly(painter, option, index)
        else:
            self.paintIconWithText(painter, option, index)

    def sizeHint(self, option: QStyleOptionViewItem, index: QModelIndex) -> QSize:
        return self.m_group.gridSize()

    def paintIconOnly(self, painter: QPainter, option: QStyleOptionViewItem, index: QModelIndex):
        style = self.m_group.style()
        sp = self.m_group.spacing()
        sp += 3
        painter.save()
        painter.setClipRect(option.rect)
        style.drawPrimitive(QStyle.PE_PanelItemViewItem, option, painter, self.m_group)

        iconRect = option.rect
        iconRect.adjust(sp, sp, -sp, -sp)
        icon = QIcon(index.data(Qt.DecorationRole).value()) # FIXME
        icon.paint(painter, iconRect, Qt.AlignCenter, QIcon.Normal, QIcon.On)
        painter.restore()

    def paintIconWithText(self, painter: QPainter, option: QStyleOptionViewItem, index: QModelIndex):
        super().paint(painter, option, index)

    def paintIconWithTextWordWrap(self, painter: QPainter, option: QStyleOptionViewItem, index: QModelIndex):
        super().paint(painter, option, index)


class RibbonGalleryGroupModel(QAbstractListModel):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_items: List[RibbonGalleryItem] = list()

    def rowCount(self, parent: QModelIndex) -> int:
        return 0 if parent.isValid() else len(self.m_items)

    def flags(self, index: QModelIndex) -> int:
        if not index.isValid() or index.row() >= len(self.m_items):
            return Qt.NoItemFlags
        return self.m_items[index.row()].flags()

    def data(self, index: QModelIndex, role: int) -> QVariant:
        if not index.isValid() or index.row() >= len(self.m_items):
            return QVariant()
        return self.m_items[index.row()].data(role)

    def index(self, row: int, column: int, parent: QModelIndex) -> QModelIndex:
        if self.hasIndex(row, column, parent):
            return self.createIndex(row, column, self.m_items[row])
        return QModelIndex()

    def setData(self, index: QModelIndex, value: QVariant, role: int) -> bool:
        if not index.isValid() or index.row() >= len(self.m_items):
            return False
        self.m_items[index.row()].setData(role, value)
        return True

    def clear(self):
        self.beginResetModel()
        self.m_items.clear()
        self.endResetModel()

    def at(self, row: int) -> RibbonGalleryItem:
        return self.m_items[row]

    def insert(self, row: int, item: RibbonGalleryItem):
        self.beginInsertRows(QModelIndex(), row, row)
        self.m_items.insert(row, item)
        self.endInsertRows()

    def take(self, row: int) -> RibbonGalleryItem:
        if row < 0 or row >= len(self.m_items):
            return None
        self.beginRemoveRows(QModelIndex(), row, row)
        item = self.m_items.pop(row)
        self.endRemoveRows()
        return item

    def append(self, item: RibbonGalleryItem):
        count = len(self.m_items)
        self.beginInsertRows(QModelIndex(), count, count + 1)
        self.m_items.append(item)
        self.endInsertRows()


class RibbonGalleryGroup(QListView):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_groupTitle = ""
        self.m_preStyle = RibbonGalleryGroup.IconWidthText
        self.m_displayRow = RibbonGalleryGroup.DisplayOneRow
        self.m_gridMinimumWidth = 0
        self.m_gridMaximumWidth = 0
        self.m_blockRecalc = False

        self.m_actionGroup = QActionGroup(self)
        self.m_actionGroup.triggered.connect(self.triggered)
        self.m_actionGroup.hovered.connect(self.hovered)

        self.setViewMode(QListView.IconMode)
        self.setResizeMode(QListView.Adjust)
        self.setSelectionRectVisible(True)
        self.setUniformItemSizes(True)
        self.setSpacing(1)
        self.setItemDelegate(RibbonGalleryGroupItemDelegate(self, self))
        self.clicked.connect(self.onItemClicked)
        self.setModel(RibbonGalleryGroupModel(self))

    def setRecalcGridSizeBlock(self, on: bool = True):
        self.m_blockRecalc = on

    def isRecalcGridSizeBlock(self) -> bool:
        return self.m_blockRecalc

    def recalcGridSize(self, *args):
        """
        recalcGridSize()
        recalcGridSize(galleryHeight: int)
        """
        if self.isRecalcGridSizeBlock():
            return
        if len(args) == 1:
            galleryHeight = args[0]
        else:
            galleryHeight = self.height()
        dr = self.getDisplayRow()
        if dr < 1:
            dr = 1
        elif dr > 3:
            dr = 3
        h = galleryHeight / dr
        if h <= 1:
            h = galleryHeight
        w = h
        if self.getGridMinimumWidth() > 0:
            if w < self.getGridMinimumWidth():
                w = self.getGridMinimumWidth()
        if self.getGridMaximumWidth() > 0:
            if w > self.getGridMaximumWidth():
                w = self.getGridMaximumWidth()
        w = round(w)
        h = round(h)
        self.setGridSize(QSize(w, h))
        shiftpix = 4
        t = self.getGalleryGroupStyle()
        spacing = self.spacing()
        if t == RibbonGalleryGroup.IconWidthText:
            textHeight = self.fontMetrics().lineSpacing()
            iconHeight = h - textHeight - 2 * spacing - shiftpix
            if iconHeight > 0:
                self.setIconSize(QSize(w - 2 * spacing - shiftpix, iconHeight))
            else:
                self.setIconSize(QSize(w - 2 * spacing - shiftpix, h - 2 * spacing - shiftpix))
        elif t == RibbonGalleryGroup.IconWithWordWrapText:
            textHeight = self.fontMetrics().lineSpacing() * 2
            iconHeight = h - textHeight
            if iconHeight > 0:
                self.setIconSize(QSize(w - 2 * spacing - shiftpix, iconHeight))
            else:
                self.setIconSize(QSize(w - 2 * spacing - shiftpix, h - 2 * spacing - shiftpix))
        else:
            self.setIconSize(QSize(w - 2 * spacing - shiftpix, h - 2 * spacing - shiftpix))

    def setGalleryGroupStyle(self, style):
        self.m_preStyle = style
        if style == RibbonGalleryGroup.IconWithWordWrapText:
            self.setWordWrap(True)
        self.recalcGridSize()

    def getGalleryGroupStyle(self) -> int:
        return self.m_preStyle

    def addItem(self, *args):
        """
        addItem(text: str, icon: QIcon)
        addItem(item: RibbonGalleryItem)
        """
        if not self.groupModel():
            return
        item = None
        arg_len = len(args)
        if arg_len == 2:
            item = RibbonGalleryItem(args[0], args[1])
        elif arg_len == 1 and isinstance(args[0], RibbonGalleryItem):
            item = args[0]
        if not item:
            return
        self.groupModel().append(item)

    def addActionItem(self, action: QAction):
        if not self.groupModel():
            return
        self.m_actionGroup.addAction(action)
        self.groupModel().append(RibbonGalleryItem(action))

    def addActionItemList(self, actions: List[QAction]):
        model = self.groupModel()
        if not model:
            return
        for a in actions:
            self.m_actionGroup.addAction(a)
            model.append(RibbonGalleryItem(a))

    def setupGroupModel(self):
        self.setModel(RibbonGalleryGroupModel(self))

    def groupModel(self) -> RibbonGalleryGroupModel:
        model = self.model()
        if isinstance(model, RibbonGalleryGroupModel):
            return model
        else:
            return None

    def setGroupTitle(self, title: str):
        self.m_groupTitle = title
        self.groupTitleChanged.emit(title)

    def getGroupTitle(self) -> str:
        return self.m_groupTitle

    def selectByIndex(self, index: int):
        model = self.groupModel()
        if not model:
            return
        idx = model.index(index, 0, QModelIndex())
        selmodel = self.selectionModel()
        if selmodel:
            selmodel.select(idx, QItemSelectionModel.SelectCurrent)

    def setDisplayRow(self, row: int):
        self.m_displayRow = row
        self.recalcGridSize()

    def getDisplayRow(self) -> int:
        return self.m_displayRow

    def setGridMinimumWidth(self, width: int):
        self.m_gridMinimumWidth = width

    def getGridMinimumWidth(self) -> int:
        return self.m_gridMinimumWidth

    def setGridMaximumWidth(self, width: int):
        self.m_gridMaximumWidth = width

    def getGridMaximumWidth(self) -> int:
        return self.m_gridMaximumWidth

    def getActionGroup(self) -> QActionGroup:
        return self.m_actionGroup

    def onItemClicked(self, index: QModelIndex):
        if not index.isValid():
            return
        itemTmp = index.internalPointer()
        if itemTmp and isinstance(itemTmp, RibbonGalleryItem):
            item: RibbonGalleryItem = itemTmp
            action = item.action()
            if action:
                action.activate(QAction.Trigger)

    def onItemEntered(self, index: QModelIndex):
        if index.isValid():
            return
        itemTmp = index.internalPointer()
        if itemTmp and isinstance(itemTmp, RibbonGalleryItem):
            item: RibbonGalleryItem = itemTmp
            action = item.action()
            if action:
                action.activate(QAction.Hover)

    # GalleryGroupStyle
    IconWidthText = 0
    IconWithWordWrapText = 1
    IconOnly = 2

    # DisplayRow
    DisplayOneRow = 1
    DisplayTwoRow = 2
    DisplayThreeRow = 3

    # signals
    groupTitleChanged = pyqtSignal(str)
    triggered = pyqtSignal(QAction)
    hovered = pyqtSignal(QAction)


class RibbonGalleryViewport(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_widgetToTitleLabel: Dict[QWidget, QLabel] = {}

        self.m_layout = QVBoxLayout(self)
        self.m_layout.setSpacing(0)
        self.m_layout.setContentsMargins(0, 0, 0, 0)

        self.setWindowFlags(Qt.Popup)
        pl = self.palette()
        pl.setBrush(QPalette.Window, pl.brush(QPalette.Base))
        self.setPalette(pl)

    def addWidget(self, w: QWidget, title: str = ""):
        if not title:
            w.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Minimum)
            self.m_layout.addWidget(w)
        else:
            label = QLabel(self)
            label.setText(title)
            label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Minimum)
            self.m_layout.addWidget(label)
            w.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Minimum)
            self.m_layout.addWidget(w)
            self.m_widgetToTitleLabel[w] = label

    def removeWidget(self, w: QWidget):
        label = self.getWidgetTitleLabel(w)
        if label:
            self.m_layout.removeWidget(label)
        self.m_layout.removeWidget(w)

    def getWidgetTitleLabel(self, w: QWidget) -> QLabel:
        return self.m_widgetToTitleLabel.get(w, None)

    def widgetTitleChanged(self, w: QWidget, title: str):
        label = self.getWidgetTitleLabel(w)
        if label:
            label.setText(title)


class RibbonGallery(QFrame):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.m_buttonUp = RibbonControlButton(self)
        self.m_buttonUp.setToolButtonStyle(Qt.ToolButtonIconOnly)
        self.m_buttonUp.setObjectName("RibbonGalleryButtonUp")
        self.m_buttonUp.setMaximumWidth(RibbonGallery.s_galleryButtonMaximumWidth)
        self.m_buttonUp.setIcon(QIcon(':/image/res/ArrowUp.png'))
        self.m_buttonUp.clicked.connect(self.pageUp)

        self.m_buttonDown = RibbonControlButton(self)
        self.m_buttonDown.setToolButtonStyle(Qt.ToolButtonIconOnly)
        self.m_buttonDown.setObjectName("RibbonGalleryButtonDown")
        self.m_buttonDown.setMaximumWidth(RibbonGallery.s_galleryButtonMaximumWidth)
        self.m_buttonDown.setIcon(QIcon(':/image/res/ArrowDown.png'))
        self.m_buttonDown.clicked.connect(self.pageDown)

        self.m_buttonMore = RibbonControlButton(self)
        self.m_buttonMore.setToolButtonStyle(Qt.ToolButtonIconOnly)
        self.m_buttonMore.setObjectName("RibbonGalleryButtonMore")
        self.m_buttonMore.setMaximumWidth(RibbonGallery.s_galleryButtonMaximumWidth)
        self.m_buttonMore.setIcon(QIcon(':/image/res/ArrowMore.png'))
        self.m_buttonMore.clicked.connect(self.showMoreDetail)

        self.triggered.connect(self.onTriggered)

        self.m_popupWidget: RibbonGalleryViewport = None
        self.m_viewportGroup: RibbonGalleryGroup = None

        self.m_btnLayout = QBoxLayout(QBoxLayout.TopToBottom)
        self.m_btnLayout.setSpacing(0)
        self.m_btnLayout.setContentsMargins(0, 0, 0, 0)
        self.m_btnLayout.addWidget(self.m_buttonUp)
        self.m_btnLayout.addWidget(self.m_buttonDown)
        self.m_btnLayout.addWidget(self.m_buttonMore)

        self.m_layout = QBoxLayout(QBoxLayout.RightToLeft)
        self.m_layout.setSpacing(0)
        self.m_layout.setContentsMargins(0, 0, 0, 0)
        self.m_layout.addLayout(self.m_btnLayout)
        self.m_layout.addStretch()
        self.setLayout(self.m_layout)

        self.setFrameShape(QFrame.Box)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.setMinimumWidth(200)

    def _addGalleryGroup(self, group: RibbonGalleryGroup):
        group.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        viewport = self._ensureGetPopupViewPort()
        viewport.addWidget(group, group.getGroupTitle())
        group.clicked.connect(self.onItemClicked)
        group.groupTitleChanged.connect(lambda title: viewport.widgetTitleChanged(group, title))
        self.setCurrentViewGroup(group)

    def addGalleryGroup(self, *args) -> Union[None, RibbonGalleryGroup]:
        """
        addGalleryGroup() -> RibbonGalleryGroup
        addGalleryGroup(group: RibbonGalleryGroup)
        """
        if len(args) == 1 and isinstance(args[0], RibbonGalleryGroup):
            group = args[0]
            self._addGalleryGroup(group)
        else:
            group = RibbonGalleryGroup(self)
            self._addGalleryGroup(group)
            return group

    def addCategoryActions(self, title: str, actions: List[QAction]) -> RibbonGalleryGroup:
        group = RibbonGalleryGroup(self)
        if title:
            group.setGroupTitle(title)
        group.addActionItemList(actions)
        self.addGalleryGroup(group)
        return group

    def currentViewGroup(self) -> RibbonGalleryGroup:
        return self.m_viewportGroup

    def setCurrentViewGroup(self, group: RibbonGalleryGroup):
        self._setViewPort(group)
        QApplication.postEvent(self, QResizeEvent(self.size(), self.size()))

    def getPopupViewPort(self) -> RibbonGalleryViewport:
        return self.m_popupWidget

    def sizeHint(self) -> QSize:
        return QSize(100, 62)

    @staticmethod
    def setGalleryButtonMaximumWidth(self, width: int):
        RibbonGallery.s_galleryButtonMaximumWidth = width

    def pageUp(self):
        if not self.m_viewportGroup:
            return
        vScrollBar = self.m_viewportGroup.verticalScrollBar()
        v = vScrollBar.value()
        v += vScrollBar.singleStep()
        vScrollBar.setValue(v)

    def pageDown(self):
        if not self.m_viewportGroup:
            return
        vScrollBar = self.m_viewportGroup.verticalScrollBar()
        v = vScrollBar.value()
        v -= vScrollBar.singleStep()
        vScrollBar.setValue(v)

    def showMoreDetail(self):
        if not self.m_popupWidget:
            return
        popupMenuSize = self.m_popupWidget.sizeHint()
        start = self.mapToGlobal(QPoint(0, 0))
        width = self.m_viewportGroup.width()
        width += self.style().pixelMetric(QStyle.PM_ScrollBarExtent)
        self.m_popupWidget.setGeometry(start.x(), start.y(), width, popupMenuSize.height())
        self.m_popupWidget.show()

    def onItemClicked(self, index: QModelIndex):
        obj = self.sender()
        if isinstance(obj, RibbonGalleryGroup):
            group: RibbonGalleryGroup = obj
            self.setCurrentViewGroup(group)
            curGroup = self.currentViewGroup()
            curGroup.scrollTo(index)
            curGroup.setCurrentIndex(index)

    def onTriggered(self, action: QAction):
        if self.m_popupWidget:
            if self.m_popupWidget.isVisible():
                self.m_popupWidget.hide()

    def resizeEvent(self, event: QResizeEvent):
        super().resizeEvent(event)
        h = self.layout().contentsRect().height()
        if self.m_viewportGroup:
            h = self.m_viewportGroup.height()
            self.m_viewportGroup.recalcGridSize()
        if self.m_popupWidget:
            lay = self.m_popupWidget.layout()
            if not lay:
                return
            c = lay.count()
            for i in range(c):
                item = lay.itemAt(i)
                if not item:
                    continue
                w = item.widget()
                if not w:
                    continue
                if isinstance(w, RibbonGalleryGroup):
                    g: RibbonGalleryGroup = w
                    g.recalcGridSize(h)

    def paintEvent(self, event: QPaintEvent):
        super().paintEvent(event)

    def _setViewPort(self, group: RibbonGalleryGroup):
        if not self.m_viewportGroup:
            self.m_viewportGroup = RibbonGalleryGroup(self)
            self.m_layout.addWidget(self.m_viewportGroup, 1)
        self.m_viewportGroup.setRecalcGridSizeBlock(True)
        self.m_viewportGroup.setGalleryGroupStyle(group.getGalleryGroupStyle())
        self.m_viewportGroup.setDisplayRow(group.getDisplayRow())
        self.m_viewportGroup.setSpacing(group.spacing())
        self.m_viewportGroup.setGridMaximumWidth(group.getGridMaximumWidth())
        self.m_viewportGroup.setGridMinimumWidth(group.getGridMinimumWidth())
        self.m_viewportGroup.setRecalcGridSizeBlock(False)
        self.m_viewportGroup.recalcGridSize(self.m_viewportGroup.height())
        self.m_viewportGroup.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.m_viewportGroup.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.m_viewportGroup.setModel(group.model())
        self.m_viewportGroup.show()

    def _ensureGetPopupViewPort(self) -> RibbonGalleryViewport:
        if not self.m_popupWidget:
            self.m_popupWidget = RibbonGalleryViewport(self)
        return self.m_popupWidget

    # signals
    triggered = pyqtSignal(QAction)
    hovered = pyqtSignal(QAction)

    s_galleryButtonMaximumWidth = 15
