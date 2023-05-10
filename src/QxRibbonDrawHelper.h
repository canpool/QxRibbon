/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"

#include <QIcon>
#include <QPixmap>
#include <QStyleOption>
#include <QStylePainter>

///
/// \brief 绘图辅助
///
class QX_RIBBON_EXPORT RibbonDrawHelper
{
public:
    RibbonDrawHelper();

    static QPixmap iconToPixmap(const QIcon &icon, QWidget *widget, const QStyleOption *opt, const QSize &icoSize);
    static void drawIcon(const QIcon &icon, QPainter *painter, const QStyleOption *opt, int x, int y, int width,
                         int height);
    static void drawIcon(const QIcon &icon, QPainter *painter, const QStyleOption *opt, const QRect &rect);
    static QSize iconActualSize(const QIcon &icon, const QStyleOption *opt, const QSize &iconSize);

    static void drawText(const QString &text, QStylePainter *painter, const QStyleOption *opt, Qt::Alignment al, int x,
                         int y, int width, int height);
    static void drawText(const QString &text, QStylePainter *painter, const QStyleOption *opt, Qt::Alignment al,
                         const QRect &rect);
};
