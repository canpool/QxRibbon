/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonContainers.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QStylePainter>


class RibbonCtrlContainerPrivate
{
public:
    RibbonCtrlContainerPrivate(RibbonCtrlContainer *p);

    void setWidget(QWidget *w);
    void takeWidget(QWidget *w);
public:
    RibbonCtrlContainer *q;
    QWidget *m_widget;
    QHBoxLayout *m_layout;
    QLabel *m_labelPixmap;
    QLabel *m_labelText;
    QIcon m_icon;
    QSize m_iconSize;
};


RibbonCtrlContainerPrivate::RibbonCtrlContainerPrivate(RibbonCtrlContainer *p)
    : m_widget(Q_NULLPTR)
    , m_iconSize(18, 18)
{
    q = p;
    m_layout = new QHBoxLayout(p);
    m_layout->setSpacing(2);
    m_layout->setObjectName(QString::fromUtf8("layout"));
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_labelPixmap = new QLabel(p);
    m_labelPixmap->setObjectName(QString::fromUtf8("labelPixmap"));
    m_labelPixmap->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_labelPixmap);

    m_labelText = new QLabel(p);
    m_labelText->setObjectName(QString::fromUtf8("labelText"));
    m_layout->addWidget(m_labelText);
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
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
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
    : QWidget(parent)
    , d(new RibbonCtrlContainerPrivate(this))
{
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
