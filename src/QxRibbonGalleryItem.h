/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"

#include <QAction>
#include <QIcon>
#include <QMap>
#include <QVariant>

class RibbonGalleryItemPrivate;

///
/// \brief 类似 QStandardItem 的 GalleryItem
///
class QX_RIBBON_EXPORT RibbonGalleryItem
{
public:
    RibbonGalleryItem();
    RibbonGalleryItem(const QString &text, const QIcon &icon);
    RibbonGalleryItem(QAction *act);
    virtual ~RibbonGalleryItem();

    void setData(int role, const QVariant &data);
    virtual QVariant data(int role) const;

    void setText(const QString &text);
    QString text() const;

    void setToolTip(const QString &text);
    QString toolTip() const;

    void setIcon(const QIcon &ico);
    QIcon icon() const;

    bool isSelectable() const;
    void setSelectable(bool isSelectable);

    bool isEnable() const;
    void setEnable(bool isEnable);

    void setFlags(Qt::ItemFlags flag);
    virtual Qt::ItemFlags flags() const;

    void setAction(QAction *act);
    QAction *action();

    void setTextAlignment(Qt::Alignment a);
    Qt::Alignment textAlignment() const;
private:
    RibbonGalleryItemPrivate *d;
};
