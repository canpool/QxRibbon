/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "QxRibbonButtonPrivate.h"

#include <QStyleOptionToolButton>

class RibbonColorButton;

class RibbonColorButtonPrivate : public RibbonButtonPrivate
{
public:
    RibbonColorButtonPrivate();

    QPixmap createIconPixmap(const QStyleOptionToolButton &opt, const QSize &iconsize) Q_DECL_OVERRIDE;
public:
    RibbonColorButton *qq;
    QColor m_color;
};
