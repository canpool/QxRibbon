/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonWindow.h"
#include "FramelessHelper.h"
#include "QxRibbonBar.h"
#include "QxRibbonManager.h"
#include "QxWindowButtonGroup.h"

#include <QApplication>
#include <QFile>
#include <QHash>
#include <QWindowStateChangeEvent>

class RibbonWindowPrivate
{
public:
    RibbonWindowPrivate(RibbonWindow *p);
    void init();
public:
    RibbonWindow *q;
    RibbonBar *m_ribbonBar;
    RibbonWindow::RibbonTheme m_theme;
    WindowButtonGroup *m_windowButtonGroup;
    FramelessHelper *m_framelessHelper;
    bool m_useRibbon;
};

RibbonWindowPrivate::RibbonWindowPrivate(RibbonWindow *p)
    : q(p)
    , m_ribbonBar(Q_NULLPTR)
    , m_theme(RibbonWindow::Office2013Theme)
    , m_windowButtonGroup(Q_NULLPTR)
    , m_framelessHelper(Q_NULLPTR)
    , m_useRibbon(true)
{
}

void RibbonWindowPrivate::init()
{
}

RibbonWindow::RibbonWindow(QWidget *parent, bool useRibbon)
    : QMainWindow(parent)
    , d(new RibbonWindowPrivate(this))
{
    d->init();
    d->m_useRibbon = useRibbon;
    if (useRibbon) {
        setRibbonTheme(ribbonTheme());
        setMenuWidget(new RibbonBar(this));
#ifdef Q_OS_UNIX
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif
    }
}

RibbonWindow::~RibbonWindow()
{
    delete d;
}

const RibbonBar *RibbonWindow::ribbonBar() const
{
    return d->m_ribbonBar;
}

RibbonBar *RibbonWindow::ribbonBar()
{
    return d->m_ribbonBar;
}

FramelessHelper *RibbonWindow::framelessHelper()
{
    return d->m_framelessHelper;
}

void RibbonWindow::setRibbonTheme(RibbonWindow::RibbonTheme theme)
{
    switch (theme) {
    case Office2013Theme:
        loadTheme(":/theme/res/office2013.qss");
        break;
    case NormalTheme:
        /* fallthrough */
    default:
        loadTheme(":/theme/res/default.qss");
        break;
    }
    d->m_theme = theme;
}

RibbonWindow::RibbonTheme RibbonWindow::ribbonTheme() const
{
    return d->m_theme;
}

bool RibbonWindow::isUseRibbon() const
{
    return d->m_useRibbon;
}

void RibbonWindow::updateWindowFlag(Qt::WindowFlags flags)
{
    if (isUseRibbon()) {
        d->m_windowButtonGroup->updateWindowFlag(flags);
    }
    repaint();
}

Qt::WindowFlags RibbonWindow::windowButtonFlags() const
{
    if (isUseRibbon()) {
        return d->m_windowButtonGroup->windowButtonFlags();
    }
    return windowFlags();
}

void RibbonWindow::setMenuWidget(QWidget *menubar)
{
    QMainWindow::setMenuWidget(menubar);
    RibbonBar *bar = qobject_cast<RibbonBar *>(menubar);

    if (bar) {
        d->m_ribbonBar = bar;
        d->m_ribbonBar->installEventFilter(this);
        if (Q_NULLPTR == d->m_framelessHelper) {
            d->m_framelessHelper = new FramelessHelper(this);
        }
        d->m_framelessHelper->setTitleHeight(d->m_ribbonBar->titleBarHeight());
        if (Q_NULLPTR == d->m_windowButtonGroup) {
            d->m_windowButtonGroup = new WindowButtonGroup(this);
        }
        QSize s = d->m_windowButtonGroup->sizeHint();
        s.setHeight(d->m_ribbonBar->titleBarHeight());
        d->m_windowButtonGroup->setFixedSize(s);
        d->m_windowButtonGroup->setWindowStates(windowState());
        d->m_useRibbon = true;
        d->m_windowButtonGroup->show();
    } else {
        d->m_ribbonBar = Q_NULLPTR;
        d->m_useRibbon = false;
        if (d->m_framelessHelper) {
            delete d->m_framelessHelper;
            d->m_framelessHelper = Q_NULLPTR;
        }
        if (d->m_windowButtonGroup) {
            d->m_windowButtonGroup->hide();
        }
    }
}

void RibbonWindow::setMenuBar(QMenuBar *menuBar)
{
    QMainWindow::setMenuBar(menuBar);
    RibbonBar *bar = qobject_cast<RibbonBar *>(menuBar);

    if (bar) {
        d->m_ribbonBar = bar;
        d->m_ribbonBar->installEventFilter(this);
        if (Q_NULLPTR == d->m_framelessHelper) {
            d->m_framelessHelper = new FramelessHelper(this);
        }
        d->m_framelessHelper->setTitleHeight(d->m_ribbonBar->titleBarHeight());
        if (Q_NULLPTR == d->m_windowButtonGroup) {
            d->m_windowButtonGroup = new WindowButtonGroup(this);
        }
        QSize s = d->m_windowButtonGroup->sizeHint();
        s.setHeight(d->m_ribbonBar->titleBarHeight());
        d->m_windowButtonGroup->setFixedSize(s);
        d->m_windowButtonGroup->setWindowStates(windowState());
        d->m_useRibbon = true;
        d->m_windowButtonGroup->show();
    } else {
        d->m_ribbonBar = Q_NULLPTR;
        d->m_useRibbon = false;
        if (d->m_framelessHelper) {
            delete d->m_framelessHelper;
            d->m_framelessHelper = Q_NULLPTR;
        }
        if (d->m_windowButtonGroup) {
            d->m_windowButtonGroup->hide();
        }
    }
}

void RibbonWindow::resizeEvent(QResizeEvent *event)
{
    if (d->m_ribbonBar) {
        if (d->m_ribbonBar->size().width() != this->size().width()) {
            d->m_ribbonBar->setFixedWidth(this->size().width());
        }
        if (d->m_windowButtonGroup) {
            d->m_ribbonBar->setWindowButtonsSize(d->m_windowButtonGroup->size());
        }
    }
    QMainWindow::resizeEvent(event);
}

bool RibbonWindow::eventFilter(QObject *obj, QEvent *e)
{
    // event post ribbonBar -> mainwindow -> framelessHelper
    if (obj == d->m_ribbonBar) {
        switch (e->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Leave:
        case QEvent::HoverMove:
        case QEvent::MouseButtonDblClick:
            QApplication::sendEvent(this, e); // post to framelessHelper
        default:
            break;
        }
    }
    return QMainWindow::eventFilter(obj, e);
}

bool RibbonWindow::event(QEvent *e)
{
    if (e) {
        switch (e->type()) {
        case QEvent::WindowStateChange: {
            if (isUseRibbon()) {
                d->m_windowButtonGroup->setWindowStates(windowState());
            }
        } break;
        default:
            break;
        }
    }
    return QMainWindow::event(e);
}

void RibbonWindow::loadTheme(const QString &themeFile)
{
    QFile file(themeFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    setStyleSheet(file.readAll());
    file.close();
}
