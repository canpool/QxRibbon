/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonContainers.h"
#include <QDebug>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QStylePainter>


class RibbonCtrlContainerPrivate
{
public:
    RibbonCtrlContainerPrivate(RibbonCtrlContainer *p);

    void init();
    void setWidget(QWidget *w);
    void takeWidget(QWidget *w);
public:
    RibbonCtrlContainer *q;
    QWidget *m_widget;
    QBoxLayout *m_layout;
    QLabel *m_labelPixmap;
    QLabel *m_labelText;
    QIcon m_icon;
    QSize m_iconSize;
    Qt::Orientation m_orientation;
};


RibbonCtrlContainerPrivate::RibbonCtrlContainerPrivate(RibbonCtrlContainer *p)
    : q(p)
    , m_widget(Q_NULLPTR)
    , m_iconSize(18, 18)
    , m_orientation(Qt::Horizontal)
{

}

void RibbonCtrlContainerPrivate::init()
{
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(2);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_labelPixmap = new QLabel(q);
    m_labelPixmap->setObjectName(QLatin1String("logo"));
    m_labelPixmap->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(m_labelPixmap);

    m_labelText = new QLabel(q);
    m_labelText->setObjectName(QLatin1String("title"));
    headerLayout->addWidget(m_labelText);

    QBoxLayout::Direction direction =
        m_orientation == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
    m_layout = new QBoxLayout(direction, q);
    m_layout->setSpacing(2);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(headerLayout);
}

void RibbonCtrlContainerPrivate::setWidget(QWidget *w)
{
    if (m_widget) {
        QWidget *oldwidget = m_widget;
        takeWidget(oldwidget);
        oldwidget->deleteLater();
    }
    m_widget = w;
    if (!w) {
        return;
    }
    if (w->parent() != q) {
        w->setParent(q);
    }
    m_layout->addWidget(w);
    QSizePolicy sizePolicy(m_orientation == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Preferred,
                           QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    w->setSizePolicy(sizePolicy);
}

void RibbonCtrlContainerPrivate::takeWidget(QWidget *w)
{
    int i = m_layout->indexOf(w);
    if (i >= 0) {
        QLayoutItem *item = m_layout->takeAt(i);
        delete item;
    }
}

RibbonCtrlContainer::RibbonCtrlContainer(QWidget *parent)
    : RibbonCtrlContainer(Qt::Horizontal, parent)
{
}

RibbonCtrlContainer::RibbonCtrlContainer(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
    , d(new RibbonCtrlContainerPrivate(this))
{
    d->m_orientation = orientation;
    d->init();
}

RibbonCtrlContainer::~RibbonCtrlContainer()
{
    delete d;
}

void RibbonCtrlContainer::setIconVisible(bool b)
{
    d->m_labelPixmap->setVisible(b);
}

void RibbonCtrlContainer::setTitleVisible(bool b)
{
    d->m_labelText->setVisible(b);
}

void RibbonCtrlContainer::setIcon(const QIcon &i)
{
    d->m_icon = i;
    d->m_labelPixmap->setPixmap(i.pixmap(d->m_iconSize));
}

QIcon RibbonCtrlContainer::icon() const
{
    return d->m_icon;
}

void RibbonCtrlContainer::setIconSize(const QSize &size)
{
    d->m_iconSize = size;
    setIcon(icon());
}

QSize RibbonCtrlContainer::iconSize() const
{
    return d->m_iconSize;
}

void RibbonCtrlContainer::setText(const QString &t)
{
    d->m_labelText->setText(t);
}

QString RibbonCtrlContainer::text() const
{
    return d->m_labelText->text();
}

void RibbonCtrlContainer::setWidget(QWidget *w)
{
    d->setWidget(w);
}

QWidget *RibbonCtrlContainer::widget() const
{
    return d->m_widget;
}

Qt::Orientation RibbonCtrlContainer::orientation() const
{
    return d->m_orientation;
}

void RibbonCtrlContainer::setOrientation(Qt::Orientation orientation)
{
    if (orientation == d->m_orientation) {
        return;
    }
    d->m_orientation = orientation;
    if (orientation == Qt::Horizontal) {
        d->m_layout->setDirection(QBoxLayout::LeftToRight);
        d->m_labelPixmap->setAlignment(Qt::AlignCenter);
    } else {
        d->m_layout->setDirection(QBoxLayout::TopToBottom);
        d->m_labelPixmap->setAlignment(Qt::AlignLeft);
    }
}
