/**
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
    void addSeparator();
    void addAction(QAction *act);
    void addWidget(QWidget *w);
    void addMenu(QMenu *m, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
private:
    RibbonQuickAccessBarPrivate *d;
};
