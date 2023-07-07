/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonGalleryItem.h"

class RibbonGalleryItemPrivate
{
public:
    RibbonGalleryItemPrivate();
public:
    QMap<int, QVariant> m_datas;
    Qt::ItemFlags m_flags;
    QAction *m_action;
};

RibbonGalleryItemPrivate::RibbonGalleryItemPrivate()
    : m_flags(Qt::ItemIsEnabled | Qt::ItemIsSelectable)
    , m_action(Q_NULLPTR)
{

}

RibbonGalleryItem::RibbonGalleryItem()
    : d(new RibbonGalleryItemPrivate)
{
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

RibbonGalleryItem::RibbonGalleryItem(const QString &text, const QIcon &icon)
    : RibbonGalleryItem()
{
    setText(text);
    setIcon(icon);
}

RibbonGalleryItem::RibbonGalleryItem(QAction *act)
    : RibbonGalleryItem()
{
    setAction(act);
}

RibbonGalleryItem::~RibbonGalleryItem()
{
    delete d;
}

void RibbonGalleryItem::setData(int role, const QVariant &data)
{
    d->m_datas[role] = data;
}

QVariant RibbonGalleryItem::data(int role) const
{
    if (d->m_action) {
        switch (role) {
        case Qt::DisplayRole:
            return d->m_action->text();

        case Qt::ToolTipRole:
            return d->m_action->toolTip();

        case Qt::DecorationRole:
            return d->m_action->icon();
        default:
            break;
        }
    }
    return d->m_datas.value(role);
}

void RibbonGalleryItem::setText(const QString &text)
{
    setData(Qt::DisplayRole, text);
}

QString RibbonGalleryItem::text() const
{
    return data(Qt::DisplayRole).toString();
}

void RibbonGalleryItem::setToolTip(const QString &text)
{
    setData(Qt::ToolTipRole, text);
}

QString RibbonGalleryItem::toolTip() const
{
    return data(Qt::ToolTipRole).toString();
}

void RibbonGalleryItem::setIcon(const QIcon &ico)
{
    setData(Qt::DecorationRole, ico);
}

QIcon RibbonGalleryItem::icon() const
{
    return qvariant_cast<QIcon>(data(Qt::DecorationRole));
}

bool RibbonGalleryItem::isSelectable() const
{
    return (d->m_flags & Qt::ItemIsSelectable);
}

void RibbonGalleryItem::setSelectable(bool isSelectable)
{
    if (isSelectable) {
        d->m_flags |= Qt::ItemIsSelectable;
    } else {
        d->m_flags &= ~Qt::ItemIsSelectable;
    }
}

bool RibbonGalleryItem::isEnable() const
{
    return (d->m_flags & Qt::ItemIsEnabled);
}

void RibbonGalleryItem::setEnable(bool isEnable)
{
    if (d->m_action) {
        d->m_action->setEnabled(isEnable);
    }

    if (isEnable) {
        d->m_flags |= Qt::ItemIsEnabled;
    } else {
        d->m_flags &= ~Qt::ItemIsEnabled;
    }
}

void RibbonGalleryItem::setFlags(Qt::ItemFlags flag)
{
    d->m_flags = flag;
    if (d->m_action) {
        d->m_action->setEnabled(flag & Qt::ItemIsEnabled);
    }
}

Qt::ItemFlags RibbonGalleryItem::flags() const
{
    return d->m_flags;
}

void RibbonGalleryItem::setAction(QAction *act)
{
    d->m_action = act;
    if (Q_NULLPTR == act) {
        return;
    }
    if (act->isEnabled()) {
        d->m_flags |= Qt::ItemIsEnabled;
    } else {
        d->m_flags &= ~Qt::ItemIsEnabled;
    }
}

QAction *RibbonGalleryItem::action()
{
    return d->m_action;
}

void RibbonGalleryItem::setTextAlignment(Qt::Alignment a)
{
    setData(Qt::TextAlignmentRole, (int)a);
}

Qt::Alignment RibbonGalleryItem::textAlignment() const
{
    return qvariant_cast<Qt::Alignment>(data(Qt::TextAlignmentRole));
}
