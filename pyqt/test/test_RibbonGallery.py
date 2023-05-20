#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MIT

from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import QApplication, QWidget, QAction

from PyRibbon.QxRibbonGallery import RibbonGallery, RibbonGalleryGroup


if __name__ == '__main__':

    app = QApplication([])
    mainWindow = QWidget()

    gallery = RibbonGallery(mainWindow)
    # gallery.resize(mainWindow.width(), gallery.size().height())
    group = gallery.addGalleryGroup()
    group.addActionItem(QAction(QIcon('res/logo.png'), 'test'))
    for i in range(10):
        group.addItem('test '+str(i), QIcon('res/logo.png'))

    group = RibbonGalleryGroup(mainWindow)
    group.setGroupTitle("title")
    group.addActionItem(QAction(QIcon('res/logo.png'), 'test'))
    gallery.addGalleryGroup(group)
    group.setGroupTitle("title changeed")

    mainWindow.setMinimumWidth(500)
    mainWindow.resize(500, 300)
    mainWindow.show()
    app.exec()
