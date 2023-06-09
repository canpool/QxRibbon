#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: GPL-3.0

from PyQt5.QtWidgets import QApplication, QMainWindow

from PyRibbon.QxRibbonBar import RibbonBar

if __name__ == '__main__':
    app = QApplication([])

    mainWindow = QMainWindow()
    ribbonBar = RibbonBar(mainWindow)
    ribbonBar.setRibbonStyle(RibbonBar.WpsLiteStyle)

    mainWindow.setMenuWidget(ribbonBar)

    ribbonBar.setContentsMargins(0, 0, 0, 0)
    ribbonBar.applicationButton().setText("File")

    page = ribbonBar.addPage('Page1')
    page = ribbonBar.addPage('Page2')

    mainWindow.setMinimumWidth(500)
    mainWindow.show()
    app.exec()
