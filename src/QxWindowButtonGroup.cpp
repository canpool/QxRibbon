﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxWindowButtonGroup.h"
#include "QxRibbonManager.h"

#include <QResizeEvent>
#include <QScopedPointer>
#include <QStyle>
#include <QToolButton>

#define MIN_BUTTON_WIDTH    30
#define MAX_BUTTON_WIDTH    30
#define CLS_BUTTON_WIDTH    40

class WindowButtonGroupPrivate
{
public:
    WindowButtonGroupPrivate(WindowButtonGroup *p);

    void setupMinimizeButton(bool on);
    void setupMaximizeButton(bool on);
    void setupCloseButton(bool on);
    void updateSize();
    void resize(QSize size);
    QSize sizeHint() const;
    int groupHeight() const;
public:
    WindowButtonGroup *q;
    WindowToolButton *m_closeButton;
    WindowToolButton *m_minimizeButton;
    WindowToolButton *m_maximizeButton;
    int m_closeStretch;
    int m_maxStretch;
    int m_minStretch;
    qreal m_iconScale;
};

WindowButtonGroupPrivate::WindowButtonGroupPrivate(WindowButtonGroup *p)
    : q(p)
    , m_closeButton(Q_NULLPTR)
    , m_minimizeButton(Q_NULLPTR)
    , m_maximizeButton(Q_NULLPTR)
    , m_closeStretch(4)
    , m_maxStretch(3)
    , m_minStretch(3)
    , m_iconScale(0.5)
{
}

void WindowButtonGroupPrivate::setupMinimizeButton(bool on)
{
    WindowButtonGroup *par = q;

    if (on) {
        if (m_minimizeButton) {
            m_minimizeButton->deleteLater();
            m_minimizeButton = Q_NULLPTR;
        }
        m_minimizeButton = new WindowToolButton(par);
        m_minimizeButton->setObjectName(QStringLiteral("qx_MinimizeWindowButton"));
        m_minimizeButton->setFixedSize(MIN_BUTTON_WIDTH, groupHeight());
        m_minimizeButton->setFocusPolicy(Qt::NoFocus);
        m_minimizeButton->setIconSize(m_minimizeButton->size() * m_iconScale);
        m_minimizeButton->show();
        par->connect(m_minimizeButton, &QAbstractButton::clicked, par, &WindowButtonGroup::minimizeWindow);
    } else {
        if (m_minimizeButton) {
            delete m_minimizeButton;
            m_minimizeButton = Q_NULLPTR;
        }
    }
    updateSize();
}

void WindowButtonGroupPrivate::setupMaximizeButton(bool on)
{
    WindowButtonGroup *par = q;

    if (on) {
        if (m_maximizeButton) {
            m_maximizeButton->deleteLater();
            m_maximizeButton = Q_NULLPTR;
        }
        m_maximizeButton = new WindowToolButton(par);
        m_maximizeButton->setObjectName(QStringLiteral("qx_MaximizeWindowButton"));
        m_maximizeButton->setFixedSize(MAX_BUTTON_WIDTH, groupHeight());
        m_maximizeButton->setCheckable(true);
        m_maximizeButton->setFocusPolicy(Qt::NoFocus);
        m_maximizeButton->setIconSize(m_maximizeButton->size() * m_iconScale);
        m_maximizeButton->show();
        par->connect(m_maximizeButton, &QAbstractButton::clicked, par, &WindowButtonGroup::maximizeWindow);
    } else {
        if (m_maximizeButton) {
            delete m_maximizeButton;
            m_maximizeButton = Q_NULLPTR;
        }
    }
    updateSize();
}

void WindowButtonGroupPrivate::setupCloseButton(bool on)
{
    WindowButtonGroup *par = q;

    if (on) {
        if (m_closeButton) {
            m_closeButton->deleteLater();
            m_closeButton = Q_NULLPTR;
        }
        m_closeButton = new WindowToolButton(par);
        m_closeButton->setObjectName(QStringLiteral("qx_CloseWindowButton"));
        m_closeButton->setFixedSize(CLS_BUTTON_WIDTH, groupHeight());
        m_closeButton->setFocusPolicy(Qt::NoFocus);
        // m_closeButton->setFlat(true);
        par->connect(m_closeButton, &QAbstractButton::clicked, par, &WindowButtonGroup::closeWindow);
        m_closeButton->setIconSize(m_closeButton->size() * m_iconScale);
        m_closeButton->show();
    } else {
        if (m_closeButton) {
            delete m_closeButton;
            m_closeButton = Q_NULLPTR;
        }
    }
    updateSize();
}

void WindowButtonGroupPrivate::updateSize()
{
    q->setFixedSize(sizeHint());
    resize(q->size());
}

void WindowButtonGroupPrivate::resize(QSize size)
{
    qreal tw = 0;

    if (m_closeButton) {
        tw += m_closeStretch;
    }
    if (m_maximizeButton) {
        tw += m_maxStretch;
    }
    if (m_minimizeButton) {
        tw += m_minStretch;
    }

    // Affected by the QSS style, the minimum width and height cannot be set,
    // so it is necessary to set fixed size before moving
    int x = 0;
    if (m_minimizeButton) {
        int w = (m_minStretch / tw) * size.width();
        // m_minimizeButton->setGeometry(x, 0, w, size.height());
        m_minimizeButton->setFixedSize(w, size.height());
        m_minimizeButton->move(x, 0);
        x += w;
    }
    if (m_maximizeButton) {
        int w = (m_maxStretch / tw) * size.width();
        // m_maximizeButton->setGeometry(x, 0, w, size.height());
        m_maximizeButton->setFixedSize(w, size.height());
        m_maximizeButton->move(x, 0);
        x += w;
    }
    if (m_closeButton) {
        int w = (m_closeStretch / tw) * size.width();
        // m_closeButton->setGeometry(x, 0, w, size.height());
        m_closeButton->setFixedSize(w, size.height());
        m_closeButton->move(x, 0);
    }
}

QSize WindowButtonGroupPrivate::sizeHint() const
{
    int width = 0;
    int height = groupHeight();

    if (m_closeButton) {
        width += CLS_BUTTON_WIDTH;
    }
    if (m_maximizeButton) {
        width += MAX_BUTTON_WIDTH;
    }
    if (m_minimizeButton) {
        width += MIN_BUTTON_WIDTH;
    }
    return QSize(width, height);
}

int WindowButtonGroupPrivate::groupHeight() const
{
    // In the WpsLiteStyle style, tabBarHeight may be smaller than titleBarHeight,
    // which causes the window buttons to cover the ribbon area, so use the smallest one
    return qMin(RibbonElementStyleOpt.titleBarHeight(), RibbonElementStyleOpt.tabBarHeight());
}

WindowToolButton::WindowToolButton(QWidget *p)
    : QPushButton(p)
{
    setFlat(true);
}

WindowButtonGroup::WindowButtonGroup(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent)
    , d(new WindowButtonGroupPrivate(this))
{
    updateWindowFlags(flags);
    if (parent) {
        parent->installEventFilter(this);
    }
}

WindowButtonGroup::~WindowButtonGroup()
{
    if (this->parent()) {
        this->parent()->removeEventFilter(this);
    }
    delete d;
}

void WindowButtonGroup::setupMinimizeButton(bool on)
{
    d->setupMinimizeButton(on);
}

void WindowButtonGroup::setupMaximizeButton(bool on)
{
    d->setupMaximizeButton(on);
}

void WindowButtonGroup::setupCloseButton(bool on)
{
    d->setupCloseButton(on);
}

void WindowButtonGroup::updateWindowFlags(Qt::WindowFlags flags)
{
    if (flags == Qt::WindowFlags()) {
        flags = parentWidget()->windowFlags();
    }
    setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint);
    setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint);
    setupCloseButton(flags & Qt::WindowCloseButtonHint);
    parentResize();
}


void WindowButtonGroup::setButtonWidthStretch(int close, int max, int min)
{
    d->m_maxStretch = max;
    d->m_minStretch = min;
    d->m_closeStretch = close;
}

void WindowButtonGroup::setIconScale(qreal iconscale)
{
    d->m_iconScale = iconscale;
}

void WindowButtonGroup::setWindowStates(Qt::WindowStates s)
{
    if (d->m_maximizeButton) {
        bool on = s.testFlag(Qt::WindowMaximized);
        d->m_maximizeButton->setChecked(on);
        d->m_maximizeButton->setToolTip(on ? tr("Restore") : tr("Maximize"));
    }
}

QSize WindowButtonGroup::sizeHint() const
{
    return d->sizeHint();
}

bool WindowButtonGroup::eventFilter(QObject *watched, QEvent *e)
{
    if (watched == parentWidget()) {
        switch (e->type()) {
        case QEvent::Resize:
            parentResize();
            break;
        default:
            break;
        }
    }
    return false; // event continue
}

void WindowButtonGroup::parentResize()
{
    QWidget *par = parentWidget();
    if (par) {
        QSize parSize = par->size();
        move(parSize.width() - width() - 1, 1);
    }
}

void WindowButtonGroup::resizeEvent(QResizeEvent *e)
{
    d->resize(e->size());
}

void WindowButtonGroup::closeWindow()
{
    if (parentWidget()) {
        parentWidget()->close();
    }
}

void WindowButtonGroup::minimizeWindow()
{
    if (parentWidget()) {
        parentWidget()->showMinimized();
    }
}

void WindowButtonGroup::maximizeWindow()
{
    QWidget *par = parentWidget();

    if (par) {
        if (par->isMaximized()) {
            par->showNormal();
        } else {
            par->showMaximized();
        }
    }
}
