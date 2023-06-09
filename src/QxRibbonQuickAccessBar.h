﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonContainers.h"

#include <QMenu>
#include <QToolButton>

class RibbonQuickAccessBarPrivate;

class QX_RIBBON_EXPORT RibbonQuickAccessBar : public RibbonCtrlContainer
{
    Q_OBJECT
public:
    RibbonQuickAccessBar(QWidget *parent = Q_NULLPTR);
    ~RibbonQuickAccessBar();
public:
    QAction *addSeparator();
    void addAction(QAction *act);
    void insertAction(QAction *before, QAction *action);
    void insertActions(QAction *before, QList<QAction *> actions);
    void removeAction(QAction *action);
    QAction *addWidget(QWidget *w);
    QAction *addMenu(QMenu *m, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
private:
    RibbonQuickAccessBarPrivate *d;
};
