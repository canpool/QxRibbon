#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

import sys
import logging

from PyQt5.QtCore import Qt, QFile, QIODevice
from PyQt5.QtGui import QIcon, QKeySequence, QColor, QFont
from PyQt5.QtWidgets import QApplication, QTextEdit, QStatusBar, QLineEdit, QRadioButton, \
    QToolButton, QCalendarWidget, QSizePolicy, QMenu, QAction, QLabel, QFontComboBox, \
    QMessageBox, QSpinBox, QButtonGroup, QFileDialog

from PyRibbon import Ribbon, RibbonWindow, RibbonBar, RibbonButtonGroup, RibbonMenu, \
    RibbonPage, RibbonQuickAccessBar, RibbonGroup, RibbonGalleryGroup, RibbonButton, \
    RibbonComboBox, RibbonLineEdit, RibbonCheckBox, RibbonCtrlContainer, RibbonApplicationButton


logging.basicConfig(format='%(asctime)s %(levelname)s: [%(filename)s:%(lineno)d]: %(message)s',
                    level=logging.DEBUG)


class MainWindow(RibbonWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_pageContext = None
        self.m_pageContext2 = None
        self.m_edit = QTextEdit(self)

        self.initWidgets()

    def initWidgets(self):
        self.setWindowTitle('pyribbon mainwindow demo')
        self.setCentralWidget(self.m_edit)
        self.setStatusBar(QStatusBar())

        ribbon = self.ribbonBar()
        ribbon.setContentsMargins(5, 0, 5, 0)
        ribbon.applicationButton().setText("File")

        pageMain = ribbon.addPage('Main')
        pageMain.setObjectName("pageMain")
        self.createPageMain(pageMain)

        # FIXME: multiple pages cannot work

        # pageOther = RibbonPage(ribbon)
        # pageOther.setPageName('Other')
        # pageOther.setObjectName('pageOther')
        # ribbon.addPage(pageOther)
        # # self.createPageOther(pageOther)

        # pageDelete = RibbonPage()
        # pageDelete.setPageName('Delete')
        # pageDelete.setObjectName('pageDelete')
        # ribbon.addPage(pageDelete)
        # # self.createPageDelete(pageDelete)
        #
        # pageSize = RibbonPage()
        # pageSize.setPageName("Size(example long page)")
        # pageSize.setObjectName("pageSize")
        # ribbon.addPage(pageSize)
        # # self.createPageSize(pageSize)

        # # pageContext created after page
        # self.createPageContext1()
        # self.createPageContext2()

        quickAccessBar = ribbon.quickAccessBar()
        self.createQuickAccessBar(quickAccessBar)
        rightBar = ribbon.rightButtonGroup()
        self.createRightButtonGroup(rightBar)

        # ribbon.raisePage(pageMain)

        self.setMinimumWidth(500)
        # self.showMaximized()
        self.setWindowIcon(QIcon('res/logo.png'))

    def createPageMain(self, page: RibbonPage):
        groupStyle = page.addGroup("ribbon style")

        actSave = self.createAction("Save", "res/save.svg")
        actSave.setShortcut(QKeySequence("Ctrl+S"))
        groupStyle.addLargeAction(actSave)
        actSave.triggered.connect(lambda: self.m_edit.append("actSave clicked"))

        actHideRibbon = self.createAction("hide ribbon", "res/hideRibbon.svg", "actHideRibobn")
        actHideRibbon.setCheckable(True)
        groupStyle.addSmallAction(actHideRibbon)
        actHideRibbon.triggered.connect(lambda b: self.ribbonBar().setMinimized(b))
        self.ribbonBar().minimizationChanged.connect(lambda minimized: actHideRibbon.setChecked(minimized))

        actShowHideButton = self.createAction("show hide button", "res/showHideButton.svg", "show hide buttong")
        actShowHideButton.setCheckable(True)
        groupStyle.addSmallAction(actShowHideButton)
        actShowHideButton.triggered.connect(lambda b: self.ribbonBar().showMinimumButton(b))
        actShowHideButton.trigger()

        actwordWrapIn2row = self.createAction("word wrap in 2row", "res/wordwrap.svg")
        actwordWrapIn2row.setCheckable(True)
        groupStyle.addSmallAction(actwordWrapIn2row)
        actwordWrapIn2row.triggered.connect(self.onActionWordWrapIn2rowTriggered)

        g = QButtonGroup(page)

        r = QRadioButton(self)
        r.setText("use office style")
        r.setObjectName("user office style")
        r.setChecked(True)
        g.addButton(r, RibbonBar.OfficeStyle)
        groupStyle.addSmallWidget(r)

        r = QRadioButton(self)
        r.setText("use wps style")
        r.setObjectName("user wps style")
        r.setChecked(False)
        g.addButton(r, RibbonBar.WpsLiteStyle)
        groupStyle.addSmallWidget(r)

        r = QRadioButton(self)
        r.setText("use office 2row style")
        r.setObjectName("user office 2row style")
        r.setChecked(False)
        g.addButton(r, RibbonBar.OfficeStyleTwoRow)
        groupStyle.addSmallWidget(r)

        r = QRadioButton(self)
        r.setText("use wps 2row style")
        r.setObjectName("user wps 2row style")
        r.setChecked(False)
        g.addButton(r, RibbonBar.WpsLiteStyleTwoRow)
        groupStyle.addSmallWidget(r)

        g.buttonClicked[int].connect(self.onStyleClicked)

        groupToolButtonStyle = page.addGroup("ribobn toolbutton style")
        menu = RibbonMenu(self)
        itemIcon = QIcon("res/item.svg")
        for i in range(5):
            a = menu.addAction(itemIcon, "item {}".format(i + 1))
            a.setObjectName("item {}".format(i + 1))

        act = self.createAction("test 1", "res/test1.svg")
        act.setMenu(menu)
        act.setToolTip("use QToolButton:MenuButtonPopup mode")
        btn = groupToolButtonStyle.addSmallAction(act)
        btn.setPopupMode(QToolButton.MenuButtonPopup)

        act = self.createAction("test 2", "res/test2.svg")
        act.setMenu(menu)
        act.setToolTip("use QToolButton:InstantPopup mode")
        btn = groupToolButtonStyle.addSmallAction(act)
        btn.setPopupMode(QToolButton.InstantPopup)

        groupToolButtonStyle.addSeparator()

        act = self.createAction("Delayed Popup", "res/folder-cog.svg")
        act.setMenu(menu)
        act.triggered.connect(self.onDelayedPopupCheckabletriggered)
        btn = groupToolButtonStyle.addLargeAction(act)
        btn.setPopupMode(QToolButton.DelayedPopup)

        act = self.createAction("Menu Button Popup", "res/folder-star.svg")
        act.setMenu(menu)
        act.triggered.connect(self.onMenuButtonPopupCheckabletriggered)
        btn = groupToolButtonStyle.addLargeAction(act)
        btn.setPopupMode(QToolButton.MenuButtonPopup)

        act = self.createAction("Instant Popup", "res/folder-stats.svg")
        act.setMenu(menu)
        act.triggered.connect(self.onInstantPopupCheckabletriggered)
        btn = groupToolButtonStyle.addLargeAction(act)
        btn.setPopupMode(QToolButton.InstantPopup)

        act = self.createAction("Delayed Popup checkable", "res/folder-table.svg")
        act.setCheckable(True)
        act.setMenu(menu)
        act.triggered.connect(self.onDelayedPopupCheckableTest)
        btn = groupToolButtonStyle.addLargeAction(act)
        btn.setPopupMode(QToolButton.DelayedPopup)

        act = self.createAction("Menu Button Popup checkable", "res/folder-checkmark.svg")
        act.setCheckable(True)
        act.setMenu(menu)
        act.triggered.connect(self.onMenuButtonPopupCheckableTest)
        btn = groupToolButtonStyle.addLargeAction(act)
        btn.setPopupMode(QToolButton.MenuButtonPopup)

        act = self.createAction("disable action", "res/disable.svg")
        act.setCheckable(True)
        act.setMenu(menu)
        act.setEnabled(False)
        groupToolButtonStyle.addLargeAction(act)

        optAct = QAction(self)
        optAct.triggered.connect(lambda: QMessageBox.information(self, "Option", "Option Action Triggered"))
        groupToolButtonStyle.addOptionAction(optAct)

        # group 2
        group2 = page.addGroup("group 2")
        actShowContext = self.createAction("show Context", "res/showContext.svg")
        actShowContext.setCheckable(True)
        actShowContext.triggered.connect(self.onShowContextPage)
        group2.addLargeAction(actShowContext)

        actDeleteContext = self.createAction("delete Context", "res/deleteContext.svg")
        actDeleteContext.triggered.connect(lambda: (self._destroyPageContext1(), actDeleteContext.setDisabled(True)))
        group2.addLargeAction(actDeleteContext)

        # group 3
        groupWidgetTest = page.addGroup("widget test")
        groupWidgetTest.setObjectName("groupWidgetTest")

        com = RibbonComboBox(self)
        com.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        com.setWindowTitle("comboBox")
        for i in range(40):
            com.addItem("RibbonComboBox test{}".format(i + 1))
        groupWidgetTest.addSmallWidget(com)

        com = RibbonComboBox(self)
        com.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        com.setWindowTitle("ComboBox Editable")
        for i in range(40):
            com.addItem("item {}".format(i + 1))
        com.setEditable(True)
        groupWidgetTest.addSmallWidget(com)

        lineEdit = RibbonLineEdit(self)
        lineEdit.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        lineEdit.setWindowTitle("Line Edit")
        lineEdit.setText("RibbonLineEdit")
        # w = lineEdit.parentWidget()
        # while w:
        #     print(w.metaObject().className())
        #     w = w.parentWidget()
        groupWidgetTest.addSmallWidget(lineEdit)

        checkBox = RibbonCheckBox(self)
        checkBox.setText("checkBox")
        groupWidgetTest.addSmallWidget(checkBox)

        groupWidgetTest.addSeparator()

        calendarWidget = QCalendarWidget(self)
        calendarWidget.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        calendarWidget.setObjectName("calendarWidget")
        groupWidgetTest.addLargeWidget(calendarWidget)
        groupWidgetTest.setExpanding()

        optAct = QAction(self)
        optAct.triggered.connect(lambda: QMessageBox.information(self, "Option", "Option Action Triggered"))
        groupWidgetTest.addOptionAction(optAct)

        groupWidgetTest.setVisible(True)

    def _destroyPageContext1(self):
        if self.m_pageContext:
            self.ribbonBar().destroyPageContext(self.m_pageContext)
            self.m_pageContext = None

    def createPageOther(self, page: RibbonPage):
        group1 = RibbonGroup("group one", page)
        group1.setObjectName("PageOther-group1")

        buttonGroup1 = RibbonButtonGroup(group1)
        buttonGroup1.addAction(self.createAction("Decrease Margin", "res/Decrease-Margin.svg"))
        buttonGroup1.addAction(self.createAction("Decrease Indent", "res/Decrease-Indent.svg"))
        buttonGroup1.addAction(self.createAction("Wrap Image Left", "res/Wrap-Image Left.svg"))
        buttonGroup1.addAction(self.createAction("Wrap Image Right", "res/Wrap-Image Right.svg"))
        group1.addWidget(buttonGroup1, RibbonGroup.Medium)

        buttonGroup2 = RibbonButtonGroup(group1)
        titleAlignment = self.createAction("Align Right", "res/Align-Right.svg")
        titleAlignment.setProperty("align", int(Qt.AlignRight | Qt.AlignVCenter))
        buttonGroup2.addAction(titleAlignment)
        titleAlignment = self.createAction("Align Left", "res/Align-Left.svg")
        titleAlignment.setProperty("align", int(Qt.AlignLeft | Qt.AlignVCenter))
        buttonGroup2.addAction(titleAlignment)
        titleAlignment = self.createAction("Align Center", "res/Align-Center.svg")
        titleAlignment.setProperty("align", int(Qt.AlignCenter))
        buttonGroup2.addAction(titleAlignment)
        buttonGroup2.actionTriggered.connect(self.onButtonGroupActionTriggered)
        group1.addWidget(buttonGroup2, RibbonGroup.Medium)

        page.addGroup(group1)

        # Gallery
        gallery = group1.addGallery()

        galleryActions = []
        galleryActions.append(self._createGalleryAction("Document File", "res/gallery/Document-File.svg"))
        galleryActions.append(self._createGalleryAction("Download File", "res/gallery/Download-File.svg"))
        galleryActions.append(self._createGalleryAction("Drive File Four Word", "res/gallery/Drive-File.svg"))
        galleryActions.append(self._createGalleryAction("Dropbox File", "res/gallery/Dropbox-File.svg"))
        galleryActions.append(self._createGalleryAction("Email File", "res/gallery/Email-File.svg"))
        galleryActions.append(self._createGalleryAction("Encode File", "res/gallery/Encode-File.svg"))
        galleryActions.append(self._createGalleryAction("Favorit File", "res/gallery/Favorit-File.svg"))
        galleryActions.append(self._createGalleryAction("File Error", "res/gallery/File-Error.svg"))
        galleryActions.append(self._createGalleryAction("File Read Only", "res/gallery/File-Readonly.svg"))
        galleryActions.append(self._createGalleryAction("File Settings", "res/gallery/File-Settings.svg"))
        galleryActions.append(self._createGalleryAction("Presentation File", "res/gallery/Presentation-File.svg"))

        galleryGroup1 = gallery.addCategoryActions("Files", galleryActions)
        galleryGroup1.setGalleryGroupStyle(RibbonGalleryGroup.IconWithWordWrapText)
        galleryGroup1.setGridMinimumWidth(80)

        galleryActions.clear()
        galleryActions.append(self._createGalleryAction("Photoshop", "res/gallery/Photoshop.svg"))
        galleryActions.append(self._createGalleryAction("Internet-Explorer", "res/gallery/Internet-Explorer.svg"))
        galleryActions.append(self._createGalleryAction("Illustrator", "res/gallery/Illustrator.svg"))
        galleryActions.append(self._createGalleryAction("Google-Maps", "res/gallery/Google-Maps.svg"))
        galleryActions.append(self._createGalleryAction("Adobe", "res/gallery/Adobe.svg"))
        galleryActions.append(self._createGalleryAction("Word", "res/gallery/Word.svg"))

        galleryGroup2 = gallery.addCategoryActions("Apps", galleryActions)
        galleryGroup2.setGridMinimumWidth(80)
        gallery.setCurrentViewGroup(galleryGroup1)

        optAct = QAction(self)
        optAct.setObjectName("debug")
        group1.addOptionAction(optAct)

        group2 = RibbonGroup("group two")
        group2.setObjectName("PageOhter-group2")
        page.addGroup(group2)

        actionRemoveAppBtn = self.createAction("remove application button", "res/remove-app-btn.svg")
        actionRemoveAppBtn.setCheckable(True)
        actionRemoveAppBtn.toggled.connect(self.onActionRemoveAppBtnTriggered)
        group2.addLargeAction(actionRemoveAppBtn)

        actionLongText = self.createAction("show very long text in a button, balabalabala etc", "res/long-text.svg",
                                           "long-text")
        group2.addLargeAction(actionLongText)

        groupStyle = RibbonGroup("style")
        groupStyle.setObjectName("PageOther-groupStyle")
        page.addGroup(groupStyle)

        actionUseQss = self.createAction("use qss", "res/useqss.svg")
        actionUseQss.triggered.connect(self.onActionUseQssTriggered)
        groupStyle.addSmallAction(actionUseQss)

        actionWindowFlagNormalButton = self.createAction("window normal button", "res/windowsflag-normal.svg")
        actionWindowFlagNormalButton.setCheckable(True)
        actionWindowFlagNormalButton.setChecked(True)
        groupStyle.addSmallAction(actionWindowFlagNormalButton)
        actionWindowFlagNormalButton.triggered.connect(self.onActionWindowFlagNormalButtonTriggered)

        groupUtf8 = RibbonGroup("中文显示测试")
        groupUtf8.setObjectName("PageOther-groupUtf8")
        page.addGroup(groupUtf8)

        actionChangeText = self.createAction("改变显示为英文", "res/chinese-char.svg", "actionChangeText")
        actionChangeText.setCheckable(True)
        actionChangeText.setChecked(False)
        groupUtf8.addLargeAction(actionChangeText)
        actionChangeText.triggered.connect(lambda on: (
            (groupUtf8.setGroupName("show chinese char"), actionChangeText.setText("show in chinese")) if on else
            (groupUtf8.setGroupName("中文显示测试"), actionChangeText.setText("改变显示为英文"))
        ))

        groupContextPage = RibbonGroup("Context Page")
        page.addGroup(groupContextPage)
        a1 = self.createAction("Context Page 1", "res/ContextPage.svg")
        a1.setCheckable(True)
        a1.triggered.connect(lambda c: self.ribbonBar().setPageContextVisible(self.m_pageContext, c))
        a2 = self.createAction("Context Page 2", "res/ContextPage.svg")
        a2.setCheckable(True)
        a2.triggered.connect(lambda c: self.ribbonBar().setPageContextVisible(self.m_pageContext2, c))
        groupContextPage.addLargeAction(a1)
        groupContextPage.addLargeAction(a2)

    def _createGalleryAction(self, text: str, iconurl: str) -> QAction:
        act = self.createAction(text, iconurl)
        act.triggered.connect(lambda: self.m_edit.append("{} triggered".format(text)))
        return act

    def createPageDelete(self, page: RibbonPage):
        group1 = RibbonGroup("group 1")
        group2 = RibbonGroup("group 2")

        actionRemoveGroup = self.createAction("remove group", "res/remove.svg")
        actionRemoveGroup.triggered.connect(lambda: page.removeGroup(group2))
        group1.addLargeAction(actionRemoveGroup)

        actionRemovePage = self.createAction("remove this page", "res/removePage.svg")
        actionRemovePage.triggered.connect(lambda: (
            self.ribbonBar().removePage(page),
            page.hide(),
            page.deleteLater(),
            actionRemovePage.setDisabled(True)
        ))
        group2.addLargeAction(actionRemovePage)

        page.addGroup(group1)
        page.addGroup(group2)

    def createPageSize(self, page: RibbonPage):
        group = page.addGroup("Font")
        buttonGroup1 = RibbonButtonGroup(group)
        buttonGroup1.setObjectName("button group1")

        labelFontSize = QLabel(buttonGroup1)
        labelFontSize.setText("self font")
        labelFontSize.setObjectName("labelFontSize")
        buttonGroup1.addWidget(labelFontSize)

        fontComWidget = QFontComboBox(buttonGroup1)
        fontComWidget.setObjectName("fontComboBox")
        fontComWidget.currentFontChanged.connect(self.onFontComWidgetCurrentFontChanged)
        buttonGroup1.addWidget(fontComWidget)

        act = group.addWidget(buttonGroup1, RibbonGroup.Medium)
        act.setObjectName(buttonGroup1.objectName())

        buttonGroup2 = RibbonButtonGroup(group)
        buttonGroup2.setObjectName("button group2")
        buttonGroup2.addAction(self.createAction("Bold", "res/bold.svg"))
        buttonGroup2.addAction(self.createAction("Italic", "res/Italic.svg"))
        buttonGroup2.addSeparator()
        buttonGroup2.addAction(self.createAction("Left alignment", "res/al-left.svg"))
        buttonGroup2.addAction(self.createAction("center alignment", "res/al-center.svg"))
        buttonGroup2.addAction(self.createAction("right alignment", "res/al-right.svg"))
        buttonGroup2.addAction(self.createAction("line up on both sides", "res/al-bothside.svg"))

        act = group.addWidget(buttonGroup2, RibbonGroup.Medium)
        act.setObjectName(buttonGroup2.objectName())

        group.addSeparator()

        actLargerFontSize = self.createAction("Larger", "res/largerFont.svg", "actLargerFontSize")
        actLargerFontSize.triggered.connect(self.onActionFontLargerTriggered)
        group.addLargeAction(actLargerFontSize)
        actSmallFontSize = self.createAction("Smaller", "res/smallFont.svg", "actSmallFontSize")
        actSmallFontSize.triggered.connect(self.onActionFontSmallerTriggered)
        group.addLargeAction(actSmallFontSize)

    def createPageContext1(self):
        ribbon = self.ribbonBar()
        self.m_pageContext = ribbon.addPageContext("context", QColor(), 1)

        page1 = self.m_pageContext.addPage("Page1")
        self.createPageContext_1(page1)

        page2 = self.m_pageContext.addPage("Page2")
        self.createPageContext_2(page2)

    def createPageContext2(self):
        ribbon = self.ribbonBar()
        self.m_pageContext2 = ribbon.addPageContext("context2", QColor(), 2)
        self.m_pageContext2.addPage("context2 Page1")
        self.m_pageContext2.addPage("context2 Page2")

    def createPageContext_1(self, page: RibbonPage):
        group = page.addGroup("show and hide test")

        actionHideGroup = self.createAction("hide group", "res/hideGroup.svg")
        actionHideGroup.setCheckable(True)
        group.addLargeAction(actionHideGroup)

        actionDisable = self.createAction("Disable", "res/enableTest.svg")
        actionDisable.setDisabled(True)
        group.addLargeAction(actionDisable)
        actionDisable.triggered.connect(lambda: actionDisable.setDisabled(True))

        actionUnlock = self.createAction("unlock", "res/unlock.svg")
        actionUnlock.setShortcut(QKeySequence("Ctrl+E"))
        group.addLargeAction(actionUnlock)
        actionUnlock.triggered.connect(lambda: (
            actionDisable.setEnabled(True),
            actionDisable.setText("Enabled"),
        ))

        actionSetTextTest = self.createAction("set text", "res/setText.svg")
        actionSetTextTest.setCheckable(True)
        actionSetTextTest.setShortcut(QKeySequence("Ctrl+D"))
        group.addLargeAction(actionSetTextTest)
        actionSetTextTest.toggled.connect(lambda b: (
            actionSetTextTest.setText("setText测试") if b else actionSetTextTest.setText("set test")
        ))

        actionShowTest = self.createAction("show beside group", "res/show.svg")
        actionShowTest.setCheckable(True)
        group.addLargeAction(actionShowTest)

        group2 = page.addGroup("show/hide")
        group2.addLargeAction(actionSetTextTest)
        actionShowTest.toggled.connect(lambda b: (
            group2.setVisible(not b),
            actionShowTest.setText("hide beside group") if b else actionShowTest.setText("show beside group"),
            self.ribbonBar().repaint()
        ))

        group3 = page.addGroup("show/hide action test")
        actionHideAction2 = self.createAction("hide action 2", "res/action.svg")
        actionHideAction4 = self.createAction("hide action 4", "res/action.svg")
        actionHideAction2.setCheckable(True)
        actionHideAction2.setChecked(True)
        actionHideAction4.setCheckable(True)
        actionHideAction4.setChecked(True)

        act2 = self.createAction("action2", "res/action2.svg")
        act3 = self.createAction("action3", "res/action3.svg")
        act4 = self.createAction("action4", "res/action4.svg")
        group3.addLargeAction(actionHideAction2)
        group3.addLargeAction(actionHideAction4)
        group3.addSmallAction(act2)
        group3.addSmallAction(act3)
        group3.addSmallAction(act4)

        actionHideAction2.triggered.connect(lambda b:
            (act2.setVisible(True), actionHideAction2.setText("hide action2")) if b else
            (act2.setVisible(False), actionHideAction2.setText("show action2"))
        )
        actionHideAction4.triggered.connect(lambda b:
            (act4.setVisible(True), actionHideAction4.setText("hide action4")) if b else
            (act4.setVisible(False), actionHideAction4.setText("show action4"))
        )

        group4 = page.addGroup("widget")
        spinbox = QSpinBox(self)
        ctrlContainer1 = RibbonCtrlContainer(group4)
        ctrlContainer1.setWidget(spinbox)
        ctrlContainer1.setText("spinbox:")
        ctrlContainer1.setIconVisible(False)
        group4.addMediumWidget(ctrlContainer1)

        linedit = QLineEdit(self)
        ctrlContainer2 = RibbonCtrlContainer(group4)
        ctrlContainer2.setWidget(linedit)
        ctrlContainer2.setText("linedit:")
        ctrlContainer2.setIconVisible(False)
        group4.addMediumWidget(ctrlContainer2)

    def createPageContext_2(self, page: RibbonPage):
        group1 = page.addGroup("删除Page测试")
        actionDeleteThisPage = self.createAction("delete this page", "res/delete.svg")
        group1.addLargeAction(actionDeleteThisPage)
        actionDeleteThisPage.triggered.connect(lambda: (
            self.ribbonBar().removePage(page), page.deleteLater()
        ))

        groupLayout = page.addGroup("特殊布局")
        groupLayout.addAction(self.createAction("Large", "res/layout.svg", "@Large1"),
                              QToolButton.InstantPopup, RibbonGroup.Large)
        groupLayout.addAction(self.createAction("Small", "res/layout.svg", "@Small1"),
                              QToolButton.InstantPopup, RibbonGroup.Small)
        groupLayout.addAction(self.createAction("Small", "res/layout.svg", "@Small2"),
                              QToolButton.InstantPopup, RibbonGroup.Small)
        groupLayout.addSeparator()
        groupLayout.addAction(self.createAction("Small", "res/layout.svg", "@Small3"),
                              QToolButton.InstantPopup, RibbonGroup.Small)
        groupLayout.addAction(self.createAction("Small", "res/layout.svg", "@Small4"),
                              QToolButton.InstantPopup, RibbonGroup.Small)
        groupLayout.addAction(self.createAction("Small", "res/layout.svg", "@Small5"),
                              QToolButton.InstantPopup, RibbonGroup.Small)
        groupLayout.addAction(self.createAction("Small", "res/layout.svg", "@Small6"),
                              QToolButton.InstantPopup, RibbonGroup.Small)
        groupLayout.addSeparator()
        groupLayout.addAction(self.createAction("Large", "res/layout.svg", "@Large2"),
                              QToolButton.InstantPopup, RibbonGroup.Large)
        groupLayout.addAction(self.createAction("Medium", "res/layout.svg", "@Medium1"),
                              QToolButton.InstantPopup, RibbonGroup.Medium)
        groupLayout.addAction(self.createAction("Medium", "res/layout.svg", "@Medium2"),
                              QToolButton.InstantPopup, RibbonGroup.Medium)
        groupLayout.addAction(self.createAction("Small", "res/layout.svg", "@Small7"),
                              QToolButton.InstantPopup, RibbonGroup.Small)
        groupLayout.addAction(self.createAction("Medium", "res/layout.svg", "@Medium3"),
                              QToolButton.InstantPopup, RibbonGroup.Medium)
        groupLayout.addAction(self.createAction("Large", "res/layout.svg", "@Large3"),
                              QToolButton.InstantPopup, RibbonGroup.Large)
        groupLayout.addAction(self.createAction("Medium", "res/layout.svg", "@Medium4"),
                              QToolButton.InstantPopup, RibbonGroup.Large)
        groupLayout.addAction(self.createAction("Medium", "res/layout.svg", "@Medium5"),
                              QToolButton.InstantPopup, RibbonGroup.Large)
        groupLayout.addAction(self.createAction("Large", "res/layout.svg", "@Large4"),
                              QToolButton.InstantPopup, RibbonGroup.Large)
        groupLayout.addSeparator()
        groupLayout.addAction(self.createAction("Medium", "res/layout.svg", "@Medium6"),
                              QToolButton.InstantPopup, RibbonGroup.Medium)
        groupLayout.addAction(self.createAction("Large", "res/layout.svg", "@Large5"),
                              QToolButton.InstantPopup, RibbonGroup.Large)
        groupLayout.addAction(self.createAction("Medium", "res/layout.svg", "@Medium7"),
                              QToolButton.InstantPopup, RibbonGroup.Medium)
        groupLayout.addAction(self.createAction("Small", "res/layout.svg", "@Small8"),
                              QToolButton.InstantPopup, RibbonGroup.Small)

    def createQuickAccessBar(self, bar: RibbonQuickAccessBar):
        bar.addAction(self.createAction("save", "res/save.svg", "save-quickbar"))
        bar.addSeparator()
        bar.addAction(self.createAction("undo", "res/undo.svg"))
        bar.addAction(self.createAction("redo", "res/redo.svg"))
        bar.addSeparator()
        m = QMenu("Presentation File", self)
        m.setIcon(QIcon("res/presentationFile.svg"))
        for i in range(10):
            m.addAction(self.createAction("file{}".format(i + 1), "res/file.svg"))
        bar.addMenu(m)

    def createRightButtonGroup(self, group: RibbonButtonGroup):
        actionHelp = self.createAction("help", "res/help.svg")
        actionHelp.triggered.connect(self.onActionHelpTriggered)
        group.addAction(actionHelp)

    def createAction(self, *args):
        """
        createAction(text: str, iconurl: str, objName: str) -> QAction
        createAction(text: str, iconurl: str) -> QAction
        """
        arg_len = len(args)
        if arg_len >= 2:
            act = QAction(self)
            act.setText(args[0])
            act.setIcon(QIcon(args[1]))
            act.setObjectName(args[2] if arg_len >= 3 else args[0])
            return act

    def onMenuButtonPopupCheckableTest(self, b: bool):
        self.m_edit.append("MenuButtonPopupCheckableTest : {}".format(b))

    def onInstantPopupCheckableTest(self, b: bool):
        self.m_edit.append("InstantPopupCheckableTest : {}".format(b))

    def onDelayedPopupCheckableTest(self, b: bool):
        self.m_edit.append("DelayedPopupCheckableTest : {}".format(b))

    def onMenuButtonPopupCheckabletriggered(self, b: bool):
        self.m_edit.append("MenuButtonPopupCheckabletriggered : {}".format(b))

    def onInstantPopupCheckabletriggered(self, b: bool):
        self.m_edit.append("InstantPopupCheckabletriggered : {}".format(b))

    def onDelayedPopupCheckabletriggered(self, b: bool):
        self.m_edit.append("DelayedPopupCheckabletriggered : {}".format(b))

    def onShowContextPage(self, on: bool):
        if not self.m_pageContext:
            self.createPageContext1()
        if on:
            self.ribbonBar().showPageContext(self.m_pageContext)
        else:
            self.ribbonBar().hidePageContext(self.m_pageContext)

    def onStyleClicked(self, style: int):
        self.ribbonBar().setRibbonStyle(style)

    def onActionHelpTriggered(self):
        QMessageBox.information(self, "about",
                                "Microsoft Ribbon for PyQt"
                                "\nVersion: {}"
                                "\nUrl: https://gitee.com/icanpool/QxRibbon"
                                .format(Ribbon.version()))

    def onActionRemoveAppBtnTriggered(self, b: bool):
        if b:
            self.ribbonBar().setApplicationButton(None)
        else:
            actionRemoveAppBtn = RibbonApplicationButton()
            actionRemoveAppBtn.setText("File")
            self.ribbonBar().setApplicationButton(actionRemoveAppBtn)

    def onActionUseQssTriggered(self):
        f = QFile("ribbon.qss")
        if not f.exists():
            f_path = QFileDialog.getOpenFileName(self, "select qss file")
            if not f_path:
                return
            f.setFileName(f_path)
        if not f.open(QIODevice.ReadWrite):
            return
        qss = f.readAll()
        self.m_edit.setText(qss)
        self.ribbonBar().setStyleSheet(qss)

    def onActionWindowFlagNormalButtonTriggered(self, b: bool):
        f = self.windowFlags()
        if b:
            f = f | Qt.WindowMinMaxButtonsHint | Qt.WindowCloseButtonHint
        else:
            f = f & ~Qt.WindowMinMaxButtonsHint & ~Qt.WindowCloseButtonHint
        self.updateWindowFlag(f)

    def onFontComWidgetCurrentFontChanged(self, f: QFont):
        self.ribbonBar().setFont(f)

    def onActionFontLargerTriggered(self):
        f = self.font()
        f.setPointSize(f.pointSize() + 1)
        self.ribbonBar().setFont(f)
        self.update()

    def onActionFontSmallerTriggered(self):
        f = self.font()
        f.setPointSize(f.pointSize() - 1)
        self.ribbonBar().setFont(f)
        self.update()

    def onActionWordWrapIn2rowTriggered(self, b: bool):
        RibbonButton.setLiteStyleEnableWordWrap(b)
        self.ribbonBar().recalc()
        self.ribbonBar().updateRibbonGeometry()

    def onButtonGroupActionTriggered(self, action: QAction):
        v = int(action.property("align"))
        self.ribbonBar().setWindowTitleAlignment(v)
        self.ribbonBar().repaint()


if __name__ == '__main__':
    app = QApplication(sys.argv)

    font = app.font()
    font.setFamily('微软雅黑')
    app.setFont(font)

    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec())
