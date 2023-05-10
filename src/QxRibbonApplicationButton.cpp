﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonApplicationButton.h"

RibbonApplicationButton::RibbonApplicationButton(QWidget *parent)
    : RibbonApplicationButton(QIcon(), QString(), parent)
{
}

RibbonApplicationButton::RibbonApplicationButton(const QString &text, QWidget *parent)
    : RibbonApplicationButton(QIcon(), text, parent)
{
}

RibbonApplicationButton::RibbonApplicationButton(const QIcon &icon, const QString &text, QWidget *parent)
    : QPushButton(icon, text, parent)
{
    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
    setMinimumWidth(40);
}
