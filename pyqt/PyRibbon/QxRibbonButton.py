#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

from PyQt5.QtCore import Qt, QSize, QRect, QEvent, QPoint
from PyQt5.QtGui import QPixmap, QPainter, QIcon, QCursor, QPalette
from PyQt5.QtWidgets import QToolButton, QAction, QStyleOptionToolButton, QWidget, \
    QStyle, QSizePolicy, QStyleOption

QX_INDICATOR_ARROW_WIDTH = 8
QX_WIDTH_HEIGHT_RATIO = 1.2


class RibbonButton(QToolButton):

    def __init__(self, *args):
        """
        RibbonButton(parent=None)
        RibbonButton(defaultAction: QAction, parent=None)
        """
        parent = None
        act: QAction = None
        arg_len = len(args)
        if arg_len > 0 and isinstance(args[0], QAction):
            parent = args[1] if arg_len >= 2 else None
            act = args[0]
        elif arg_len > 0:
            parent = args[0]
        super().__init__(parent)

        self.m_buttonType = RibbonButton.LargeButton
        self.m_largeButtonType = RibbonButton.Normal
        self.m_mouseOnSubControl = False
        self.m_menuButtonPressed = False
        self.m_isWordWrap = False
        self.m_iconAndTextSpace = 2
        self.m_iconRect = QRect()
        self.m_textRect = QRect()

        if act:
            self.setDefaultAction(act)
        self.setAutoRaise(True)
        self.setButtonType(RibbonButton.SmallButton)

    def buttonType(self) -> int:
        return self.m_buttonType

    def setButtonType(self, button_type):
        self.m_buttonType = button_type
        if RibbonButton.LargeButton == button_type:
            self.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
            self.setSizePolicy(QSizePolicy.Maximum, QSizePolicy.Expanding)
        else:
            self.setToolButtonStyle(Qt.ToolButtonTextBesideIcon)
            self.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Maximum)
        self.setMouseTracking(True)

    def sizeHint(self) -> QSize:
        s = super().sizeHint()
        opt = QStyleOptionToolButton()
        self.initStyleOption(opt)
        if RibbonButton.LargeButton == self.m_buttonType:
            # 计算最佳大小
            if (RibbonButton.Normal == self.m_largeButtonType) or \
                    (RibbonButton.Lite == self.m_largeButtonType and RibbonButton.s_liteStyleEnableWordWrap):
                if s.width() > s.height() * QX_WIDTH_HEIGHT_RATIO:
                    fm = self.fontMetrics()
                    textRange = QRect(0, 0, s.width(), s.height())
                    maxTryCount = 3
                    tryCount = 0
                    alignment = Qt.TextShowMnemonic | Qt.TextWordWrap
                    while tryCount < maxTryCount:
                        textRange.setWidth(int(s.width() / 2 + (s.width() / 2) * (tryCount / maxTryCount)))
                        textRange = fm.boundingRect(textRange, alignment, self.text())
                        if textRange.height() <= fm.lineSpacing() * 2:
                            self.m_isWordWrap = (textRange.height() > fm.lineSpacing())
                            break
                        tryCount += 1
                    s.setWidth(textRange.width() + self.m_iconAndTextSpace * 2)
                    if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or \
                            (opt.features & QStyleOptionToolButton.HasMenu):
                        if self.largeButtonType() == RibbonButton.Lite or self.m_isWordWrap:
                            s.setWidth(s.width() + QX_INDICATOR_ARROW_WIDTH)
                else:
                    self.m_isWordWrap = '\n' in self.text()
                    if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or \
                            (opt.features & QStyleOptionToolButton.HasMenu):
                        # 如果有菜单
                        if self.m_isWordWrap:
                            s.setWidth(s.width() + QX_INDICATOR_ARROW_WIDTH)
            else:
                if s.width() > s.height() * 1.5:
                    s.setWidth(int(s.height() * 1.5))
                if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or \
                        (opt.features & QStyleOptionToolButton.HasMenu):
                    s.setWidth(s.width() + QX_INDICATOR_ARROW_WIDTH)
        else:
            # 在仅有图标的小模式显示时，预留一个下拉箭头位置
            if self.toolButtonStyle() == Qt.ToolButtonIconOnly:
                if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or \
                        (opt.features & QStyleOptionToolButton.HasMenu):
                    s.setWidth(s.width() + QX_INDICATOR_ARROW_WIDTH)
        return s

    def minimumSizeHint(self) -> QSize:
        return self.sizeHint()

    def setLargeButtonType(self, large_type):
        self.m_largeButtonType = large_type

    def largeButtonType(self) -> int:
        return self.m_largeButtonType

    @staticmethod
    def setToolButtonTextShift(on: bool):
        RibbonButton.s_isToolButtonTextShift = on

    @staticmethod
    def isToolButtonTextShift() -> bool:
        return RibbonButton.s_isToolButtonTextShift

    @staticmethod
    def setLiteStyleEnableWordWrap(on: bool):
        RibbonButton.s_liteStyleEnableWordWrap = on

    @staticmethod
    def isLiteStyleEnableWordWrap() -> bool:
        return RibbonButton.s_liteStyleEnableWordWrap

    def event(self, e: QEvent) -> bool:
        eList = (
            QEvent.WindowDeactivate,
            QEvent.ActionChanged,
            QEvent.ActionRemoved,
            QEvent.ActionAdded,
        )
        if e.type() in eList:
            self.m_mouseOnSubControl = False
        return super().event(e)

    def changeEvent(self, e: QEvent):
        if e:
            if e.type() == QEvent.FontChange:
                opt = QStyleOptionToolButton()
                self.initStyleOption(opt)
                self._calcIconAndTextRect(opt)
        super().changeEvent(e)

    def paintEvent(self, e: QEvent):
        if self.m_buttonType == self.LargeButton:
            self._drawLargeButton(e)
        elif self.m_buttonType == self.SmallButton:
            self._drawSmallButton(e)

    def resizeEvent(self, e):
        super().resizeEvent(e)
        opt = QStyleOptionToolButton()
        self.initStyleOption(opt)
        self._calcIconAndTextRect(opt)

    def mouseMoveEvent(self, e):
        isMouseOnSubControl = False
        if self.m_iconRect.isValid():
            isMouseOnSubControl = not self.m_iconRect.contains(e.pos())
        if self.m_mouseOnSubControl != isMouseOnSubControl:
            self.m_mouseOnSubControl = isMouseOnSubControl
            self.update()
        super().mouseMoveEvent(e)

    def mousePressEvent(self, e):
        if e.button() == Qt.LeftButton and self.popupMode() == QToolButton.MenuButtonPopup:
            if self.m_buttonType == self.LargeButton:
                popupr = self.rect().adjusted(0, int(self.height() / 2), 0, 0)
                if popupr.isValid() and popupr.contains(e.pos()):
                    self.m_menuButtonPressed = True
                    self.showMenu()
                    return
            elif self.m_iconRect.isValid() and not self.m_iconRect.contains(e.pos()):
                self.m_menuButtonPressed = True
                self.showMenu()
                return
        self.m_menuButtonPressed = False
        super().mousePressEvent(e)

    def mouseReleaseEvent(self, e):
        super().mouseReleaseEvent(e)
        self.m_menuButtonPressed = False

    def focusOutEvent(self, e):
        super().focusOutEvent(e)
        self.m_mouseOnSubControl = False

    def leaveEvent(self, e):
        self.m_mouseOnSubControl = False
        super().leaveEvent(e)

    def hitButton(self, pos: QPoint) -> bool:
        if super().hitButton(pos):
            return not self.m_menuButtonPressed
        return False

    def _calcIconAndTextRect(self, opt: QStyleOptionToolButton):
        if RibbonButton.LargeButton == self.m_buttonType:
            self.m_iconRect = opt.rect.adjusted(self.m_iconAndTextSpace, self.m_iconAndTextSpace,
                                                -self.m_iconAndTextSpace, -self.m_iconAndTextSpace)
            if opt.toolButtonStyle != Qt.ToolButtonIconOnly:
                fm = self.fontMetrics()
                if RibbonButton.Normal == self.m_largeButtonType or \
                        (RibbonButton.Lite == self.m_largeButtonType and RibbonButton.s_liteStyleEnableWordWrap):
                    maxHeight = opt.rect.height() - (fm.lineSpacing() * 2) - \
                                2 * self.m_iconAndTextSpace - self.m_iconAndTextSpace
                else:
                    maxHeight = opt.rect.height() - (fm.lineSpacing() * 1.2) - \
                                2 * self.m_iconAndTextSpace - self.m_iconAndTextSpace
                self.m_iconRect.setHeight(round(maxHeight))
                self.m_textRect.setRect(self.m_iconAndTextSpace,
                                        self.m_iconRect.bottom() + self.m_iconAndTextSpace,
                                        self.m_iconRect.width(),
                                        opt.rect.bottom() - self.m_iconRect.bottom() - self.m_iconAndTextSpace)
                if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or \
                        (opt.features & QStyleOptionToolButton.HasMenu):
                    if self.m_isWordWrap:
                        self.m_textRect.adjust(0, 0, -QX_INDICATOR_ARROW_WIDTH, 0)
                    elif RibbonButton.Lite == self.m_largeButtonType and not RibbonButton.s_liteStyleEnableWordWrap:
                        self.m_textRect.adjust(0, 0, -QX_INDICATOR_ARROW_WIDTH, 0)
            else:
                self.m_textRect = QRect()
        else:
            # small button
            if opt.toolButtonStyle == Qt.ToolButtonIconOnly:
                self.m_iconRect = opt.rect.adjusted(self.m_iconAndTextSpace, self.m_iconAndTextSpace,
                                                    -self.m_iconAndTextSpace, -self.m_iconAndTextSpace)
                if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or \
                        (opt.features & QStyleOptionToolButton.HasMenu):
                    self.m_iconRect.adjust(0, 0, -QX_INDICATOR_ARROW_WIDTH, 0)
                self.m_textRect = QRect()
            else:
                self.m_iconRect = QRect(self.m_iconAndTextSpace, self.m_iconAndTextSpace,
                                        max(opt.rect.height(), opt.iconSize.width()) - 2 * self.m_iconAndTextSpace,
                                        opt.rect.height() - 2 * self.m_iconAndTextSpace)
                self.m_textRect.setRect(self.m_iconRect.right() + self.m_iconAndTextSpace, self.m_iconAndTextSpace,
                                        opt.rect.right() - self.m_iconRect.right() - 2 * self.m_iconAndTextSpace,
                                        opt.rect.height() - 2 * self.m_iconAndTextSpace)
                if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or \
                        (opt.features & QStyleOptionToolButton.HasMenu):
                    self.m_textRect.adjust(0, 0, -QX_INDICATOR_ARROW_WIDTH, 0)
        # 纯文本的文字位置
        if Qt.ToolButtonTextOnly == self.toolButtonStyle() or self.icon().isNull():
            self.m_textRect = opt.rect.adjusted(self.m_iconAndTextSpace, self.m_iconAndTextSpace,
                                                -self.m_iconAndTextSpace, -self.m_iconAndTextSpace)

    def _adjustedTextRect(self, opt: QStyleOptionToolButton) -> QRect:
        rect = self.m_textRect
        if RibbonButton.s_isToolButtonTextShift:
            x = y = 0
            if opt.state & (QStyle.State_Sunken | QStyle.State_On):
                x = self.style().pixelMetric(QStyle.PM_ButtonShiftHorizontal, opt, self)
                y = self.style().pixelMetric(QStyle.PM_ButtonShiftVertical, opt, self)
            rect.translate(x, y)
        return rect

    def _calcIndicatorArrowDownRect(self, opt: QStyleOptionToolButton) -> QRect:
        """sub control 的下拉箭头的位置"""
        rect = QRect(opt.rect)
        if RibbonButton.LargeButton == self.m_buttonType:
            if RibbonButton.Lite == self.m_largeButtonType and not RibbonButton.s_liteStyleEnableWordWrap:
                rect.setRect(self.m_textRect.right(), self.m_textRect.top(),
                             opt.rect.right() - self.m_iconAndTextSpace - self.m_textRect.right(),
                             self.m_textRect.height())
            elif self.m_isWordWrap:
                rect.setRect(self.m_textRect.right(), int(self.m_textRect.top() + self.m_textRect.height() / 2),
                             opt.rect.right() - self.m_iconAndTextSpace - self.m_textRect.right(),
                             int(self.m_textRect.height() / 2))
            else:
                rect.setRect(self.m_textRect.left(), self.m_textRect.bottom() - QX_INDICATOR_ARROW_WIDTH,
                             self.m_textRect.width(), QX_INDICATOR_ARROW_WIDTH)
        else:
            rect.setRect(rect.width() - QX_INDICATOR_ARROW_WIDTH - self.m_iconAndTextSpace, self.m_iconAndTextSpace,
                         QX_INDICATOR_ARROW_WIDTH, rect.height() - 2 * self.m_iconAndTextSpace)
        return rect

    def _createIconPixmap(self, opt: QStyleOptionToolButton, icon_size: QSize) -> QPixmap:
        if not opt.icon.isNull():  # 有图标
            state = QIcon.On if opt.state & QStyle.State_On else QIcon.Off
            if not (opt.state & QStyle.State_Enabled):
                mode = QIcon.Disabled
            elif (opt.state & QStyle.State_MouseOver) and (opt.state & QStyle.State_AutoRaise):
                mode = QIcon.Active
            else:
                mode = QIcon.Normal
            return opt.icon.pixmap(self.window().windowHandle(), icon_size, mode, state)
        return QPixmap()

    def _drawLargeButton(self, e: QEvent):
        p = QPainter(self)
        opt = QStyleOptionToolButton()
        self.initStyleOption(opt)
        if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or (opt.features & QStyleOptionToolButton.HasMenu):
            if not self.rect().contains(self.mapFromGlobal(QCursor.pos())):
                opt.state &= ~QStyle.State_MouseOver

        autoRaise = opt.state & QStyle.State_AutoRaise
        bFlags = opt.state & ~QStyle.State_Sunken
        if autoRaise and (not (bFlags & QStyle.State_MouseOver) or not (bFlags & QStyle.State_Enabled)):
            bFlags &= ~QStyle.State_Raised
        if opt.state & QStyle.State_Sunken:
            if opt.activeSubControls & QStyle.SC_ToolButton:
                bFlags |= QStyle.State_Sunken
            elif opt.activeSubControls & QStyle.SC_ToolButtonMenu:
                bFlags |= QStyle.State_MouseOver

        # 绘制背景
        tool = QStyleOption(0)
        tool.palette = opt.palette
        drawStyle = QStyle.PE_PanelButtonTool if autoRaise else QStyle.PE_PanelButtonBevel
        if (opt.subControls & QStyle.SC_ToolButton) and (opt.features & QStyleOptionToolButton.MenuButtonPopup):
            tool.rect = QRect(opt.rect)
            tool.state = bFlags
            self.style().drawPrimitive(QStyle.PE_PanelButtonTool, tool, p, self)
            opt.activeSubControls &= QStyle.SC_ToolButtonMenu
            if opt.activeSubControls:
                pass
            else:
                if tool.state & QStyle.State_MouseOver:
                    if self.m_mouseOnSubControl:
                        # 鼠标在文字区，把图标显示为正常
                        tool.rect = self.m_iconRect
                        tool.state = QStyle.State_Raised  # 把图标区域显示为正常
                    else:
                        # 鼠标在图标区，把文字显示为正常
                        tool.state = QStyle.State_Raised  # 把图标区域显示为正常
                        tool.rect.setRect(self.m_textRect.x(), self.m_textRect.y(),
                                          tool.rect.width() - 2 * self.m_iconAndTextSpace, self.m_textRect.height())
                    self.style().drawPrimitive(drawStyle, tool, p, self)
        elif (opt.subControls & QStyle.SC_ToolButton) and (opt.features & QStyleOptionToolButton.HasMenu):
            tool.rect = QRect(opt.rect)
            tool.state = bFlags
            self.style().drawPrimitive(drawStyle, tool, p, self)
        elif opt.subControls & QStyle.SC_ToolButton:
            tool.rect = QRect(opt.rect)
            tool.state = bFlags
            if opt.state & QStyle.State_Sunken:
                tool.state &= ~QStyle.State_MouseOver
            self.style().drawPrimitive(drawStyle, tool, p, self)
        # TODO: draw focus
        self._drawIconAndLabel(p, opt)

    def _drawSmallButton(self, e: QEvent):
        p = QPainter(self)
        opt = QStyleOptionToolButton()
        self.initStyleOption(opt)
        if (opt.features & QStyleOptionToolButton.MenuButtonPopup) or (opt.features & QStyleOptionToolButton.HasMenu):
            if not self.rect().contains(self.mapFromGlobal(QCursor.pos())):
                opt.state &= ~QStyle.State_MouseOver

        autoRaise = bool(opt.state & QStyle.State_AutoRaise)
        bFlags = opt.state & ~QStyle.State_Sunken
        mFlags = bFlags
        if autoRaise and (not (bFlags & QStyle.State_MouseOver) or not (bFlags & QStyle.State_Enabled)):
            bFlags &= ~QStyle.State_Raised
        if opt.state & QStyle.State_Sunken:
            if opt.activeSubControls & QStyle.SC_ToolButton:
                bFlags |= QStyle.State_Sunken
                mFlags |= QStyle.State_MouseOver | QStyle.State_Sunken
            elif opt.activeSubControls & QStyle.SC_ToolButtonMenu:
                bFlags |= QStyle.State_MouseOver
                mFlags |= QStyle.State_Sunken
        # 绘制背景
        tool = QStyleOption(0)
        tool.palette = opt.palette

        drawStyle = QStyle.PE_PanelButtonTool if autoRaise else QStyle.PE_PanelButtonBevel
        if (opt.subControls & QStyle.SC_ToolButton) and (opt.features & QStyleOptionToolButton.MenuButtonPopup):
            tool.rect = QRect(opt.rect)
            tool.state = bFlags
            opt.activeSubControls = QStyle.SC_ToolButtonMenu & opt.activeSubControls
            if opt.activeSubControls:
                # 菜单激活
                self.style().drawPrimitive(QStyle.PE_PanelButtonTool, tool, p, self)
            else:
                self.style().drawPrimitive(QStyle.PE_PanelButtonTool, tool, p, self)
                if tool.state & QStyle.State_MouseOver:
                    if self.m_mouseOnSubControl:
                        # 鼠标在文字区，把图标显示为正常
                        tool.rect = self.m_iconRect
                        tool.state = QStyle.State_Raised  # 把图标区域显示为正常
                    else:
                        # 鼠标在图标区，把文字显示为正常
                        tool.state = QStyle.State_Raised  # 把图标区域显示为正常
                        tool.rect = opt.rect.adjusted(self.m_iconRect.width() + self.m_iconAndTextSpace,
                                                      self.m_iconAndTextSpace,
                                                      -self.m_iconAndTextSpace, -self.m_iconAndTextSpace)
                    self.style().drawPrimitive(drawStyle, tool, p, self)
        elif (opt.subControls & QStyle.SC_ToolButton) and (opt.features & QStyleOptionToolButton.HasMenu):
            tool.rect = QRect(opt.rect)
            tool.state = bFlags
            self.style().drawPrimitive(drawStyle, tool, p, self)
        elif opt.subControls & QStyle.SC_ToolButton:
            tool.rect = QRect(opt.rect)
            tool.state = bFlags
            if opt.state & QStyle.State_Sunken:
                tool.state &= ~QStyle.State_MouseOver
            self.style().drawPrimitive(drawStyle, tool, p, self)

        self._drawIconAndLabel(p, opt)

    def _drawIconAndLabel(self, p: QPainter, opt: QStyleOptionToolButton):
        pm = self._createIconPixmap(opt, self.m_iconRect.size())
        if self.m_buttonType == RibbonButton.LargeButton:
            # 绘制图标和文字
            textRect = self._adjustedTextRect(opt)
            hasArrow = opt.features & QStyleOptionToolButton.Arrow
            if ((not hasArrow) and opt.icon.isNull() and opt.text) or opt.toolButtonStyle == Qt.ToolButtonTextOnly:
                # 只有文字模式
                alignment = Qt.AlignCenter | Qt.TextShowMnemonic | Qt.TextWordWrap  # 纯文本下，居中对齐,换行
                if not self.style().styleHint(QStyle.SH_UnderlineShortcut, opt, self):
                    alignment = alignment | Qt.TextHideMnemonic
                p.setFont(opt.font)
                self.style().drawItemText(p, textRect, alignment, opt.palette,
                                          opt.state & QStyle.State_Enabled, opt.text, QPalette.ButtonText)
                return
            if opt.toolButtonStyle != Qt.ToolButtonIconOnly:
                # 文本加图标情况
                p.setFont(opt.font)
                alignment = Qt.TextShowMnemonic | Qt.TextWordWrap  # 换行
                if not self.style().styleHint(QStyle.SH_UnderlineShortcut, opt, self):
                    alignment |= Qt.TextHideMnemonic
                # 文字在icon下，先绘制图标
                if not hasArrow:
                    self.style().drawItemPixmap(p, self.m_iconRect, Qt.AlignCenter, pm)
                else:
                    self._drawArrow(self.style(), opt, self.m_iconRect, p, self)
                if self.m_largeButtonType == RibbonButton.Normal:
                    alignment = (Qt.AlignHCenter | Qt.AlignTop) | alignment
                else:
                    alignment |= Qt.AlignCenter
                if RibbonButton.Lite == self.m_largeButtonType and not RibbonButton.s_liteStyleEnableWordWrap:
                    opt.text = self.fontMetrics().elidedText(opt.text, Qt.ElideRight,
                                                             textRect.width(), Qt.TextShowMnemonic)
                # 再绘制文本，对于Normal模式下的 LargeButton，如果有菜单，箭头将在文本旁边
                self.style().drawItemText(p, QStyle.visualRect(opt.direction, opt.rect, textRect), alignment,
                                          opt.palette, opt.state & QStyle.State_Enabled, opt.text, QPalette.ButtonText)
            else:
                # 只有图标情况
                if not hasArrow:
                    self.style().drawItemPixmap(p, self.m_iconRect, Qt.AlignCenter, pm)
                else:
                    self._drawArrow(self.style(), opt, opt.rect, p, self)
            # 绘制sub control 的下拉箭头
            if opt.features & QStyleOptionToolButton.MenuButtonPopup or \
                    opt.features & QStyleOptionToolButton.HasMenu:
                opt.rect = self._calcIndicatorArrowDownRect(opt)
                self.style().drawPrimitive(QStyle.PE_IndicatorArrowDown, opt, p, self)
        else:
            if opt.icon.isNull():
                # 只有文字
                alignment = Qt.TextShowMnemonic
                if not self.style().styleHint(QStyle.SH_UnderlineShortcut, opt, self):
                    alignment |= Qt.TextHideMnemonic
                self.style().drawItemText(p, QStyle.visualRect(opt.direction, opt.rect,
                                                               opt.rect.adjusted(2, 1, -2, -1)),
                                          alignment, opt.palette, opt.state & QStyle.State_Enabled,
                                          opt.text, QPalette.ButtonText)
            elif opt.toolButtonStyle != Qt.ToolButtonIconOnly:
                # 文本加图标情况
                p.save()
                p.setFont(opt.font)
                pr = self.m_iconRect  # 图标区域
                tr = opt.rect.adjusted(pr.width() + 2, 0, -1, 0)  # 文本区域

                alignment = Qt.TextShowMnemonic
                if not self.style().styleHint(QStyle.SH_UnderlineShortcut, opt, self):
                    alignment |= Qt.TextHideMnemonic
                # ribbonButton在小图标下，不支持ToolButtonTextUnderIcon
                if opt.toolButtonStyle != Qt.ToolButtonTextUnderIcon:
                    self.style().drawItemPixmap(p, QStyle.visualRect(opt.direction, opt.rect, pr), Qt.AlignCenter, pm)
                    alignment = alignment | Qt.AlignLeft | Qt.AlignVCenter
                # 绘制文本
                self.style().drawItemText(p, QStyle.visualRect(opt.direction, opt.rect, tr), alignment, opt.palette,
                                          opt.state & QStyle.State_Enabled, opt.text, QPalette.ButtonText)
                p.restore()
            else:
                # 只有图标情况
                self.style().drawItemPixmap(p, self.m_iconRect, Qt.AlignCenter, pm)

            # 绘制sub control 的下拉箭头
            if opt.features & QStyleOptionToolButton.MenuButtonPopup or \
                    opt.features & QStyleOptionToolButton.HasMenu:
                opt.rect = self._calcIndicatorArrowDownRect(opt)
                self.style().drawPrimitive(QStyle.PE_IndicatorArrowDown, opt, p, self)

    @staticmethod
    def _drawArrow(style: QStyle, opt: QStyleOptionToolButton, rect: QRect,
                   painter: QPainter, widget: QWidget = None):
        if opt.arrowType == Qt.LeftArrow:
            pe = QStyle.PE_IndicatorArrowLeft
        elif opt.arrowType == Qt.RightArrow:
            pe = QStyle.PE_IndicatorArrowRight
        elif opt.arrowType == Qt.UpArrow:
            pe = QStyle.PE_IndicatorArrowUp
        elif opt.arrowType == Qt.DownArrow:
            pe = QStyle.PE_IndicatorArrowDown
        else:
            return
        arrowOpt: QStyleOption = opt
        arrowOpt.rect = rect
        style.drawPrimitive(pe, arrowOpt, painter, widget)

    # RibbonButtonType
    LargeButton = 0
    SmallButton = 1

    # LargeButtonType
    Normal = 0
    Lite = 1

    s_isToolButtonTextShift = False
    s_liteStyleEnableWordWrap = False


if __name__ == '__main__':
    from PyQt5.QtWidgets import QApplication
    from QxRibbonControls import RibbonMenu

    app = QApplication([])
    mainWindow = QWidget()

    menu = RibbonMenu(mainWindow)
    menu.addAction(QIcon("res/logo.png"), '1')
    menu.addAction(QIcon("res/logo.png"), '2')
    menu.addAction(QIcon("res/logo.png"), '3')

    # act = QAction('Test', mainWindow)
    btn = RibbonButton(mainWindow)
    btn.setFocusPolicy(Qt.NoFocus)
    # btn.setButtonType(RibbonButton.SmallButton)
    btn.setButtonType(RibbonButton.LargeButton)
    btn.setLargeButtonType(RibbonButton.Normal)
    # btn.setDefaultAction(act)
    btn.setMenu(menu)

    btn.setText('Ribbon Button demo')
    btn.setIcon(QIcon('res/logo.png'))
    btn.setFixedHeight(78)
    btn.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
    btn.setPopupMode(QToolButton.MenuButtonPopup)

    mainWindow.setMinimumWidth(50)
    mainWindow.resize(300, 200)
    mainWindow.show()
    app.exec()
