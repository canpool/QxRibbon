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
    void setMenuWidget(QWidget *menuBar);
    void destroyFrameless();
    void setFrameless(bool frameless);
    void resizeRibbon();
public:
    RibbonWindow *q;
    RibbonBar *m_ribbonBar;
    RibbonWindow::RibbonTheme m_theme;
    WindowButtonGroup *m_windowButtonGroup;
    FramelessHelper *m_framelessHelper;
    bool m_useRibbon;
    bool m_frameless;
};

RibbonWindowPrivate::RibbonWindowPrivate(RibbonWindow *p)
    : q(p)
    , m_ribbonBar(Q_NULLPTR)
    , m_theme(RibbonWindow::Office2013Theme)
    , m_windowButtonGroup(Q_NULLPTR)
    , m_framelessHelper(Q_NULLPTR)
    , m_useRibbon(true)
    , m_frameless(true)
{
}

void RibbonWindowPrivate::init()
{
}

void RibbonWindowPrivate::setMenuWidget(QWidget *menuBar)
{
    RibbonBar *bar = qobject_cast<RibbonBar *>(menuBar);

    if (bar) {
        m_ribbonBar = bar;
        m_ribbonBar->installEventFilter(q);
        m_useRibbon = true;
    } else {
        m_ribbonBar = Q_NULLPTR;
        m_useRibbon = false;
    }
    setFrameless(m_frameless);
}

void RibbonWindowPrivate::destroyFrameless()
{
    if (m_framelessHelper) {
        delete m_framelessHelper;
        m_framelessHelper = Q_NULLPTR;
    }
    if (m_windowButtonGroup) {
        m_windowButtonGroup->hide();
        delete m_windowButtonGroup;
        m_windowButtonGroup = Q_NULLPTR;
    }
}

void RibbonWindowPrivate::setFrameless(bool frameless)
{
    if (m_useRibbon) {
        if (frameless) {
            if (Q_NULLPTR == m_framelessHelper) {
                m_framelessHelper = new FramelessHelper(q);
            }
            m_framelessHelper->setTitleHeight(m_ribbonBar->titleBarHeight());
            if (Q_NULLPTR == m_windowButtonGroup) {
                m_windowButtonGroup = new WindowButtonGroup(q);
            }
            QSize s = m_windowButtonGroup->sizeHint();
            s.setHeight(m_ribbonBar->titleBarHeight());
            m_windowButtonGroup->setFixedSize(s);
            m_windowButtonGroup->setWindowStates(q->windowState());
            // see also parentResize, using move instead of parent resize event when frameless toggled
            m_windowButtonGroup->move(q->width() - s.width() - 1, 1);
            m_windowButtonGroup->show();

#ifdef Q_OS_UNIX
            q->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#else
            q->setWindowFlags(q->windowFlags() | Qt::FramelessWindowHint);
#endif
        } else {
            destroyFrameless();
            q->setWindowFlags(q->windowFlags() & ~Qt::FramelessWindowHint);
        }
    } else {
        destroyFrameless();
    }
}

void RibbonWindowPrivate::resizeRibbon()
{
    if (m_windowButtonGroup) {
        m_ribbonBar->setWindowButtonsSize(m_windowButtonGroup->size());
    } else {
        m_ribbonBar->setWindowButtonsSize(QSize(0, 0));
    }
    QApplication::postEvent(m_ribbonBar, new QResizeEvent(m_ribbonBar->size(),
                                                          m_ribbonBar->size()));
}

RibbonWindow::RibbonWindow(QWidget *parent, bool useRibbon)
    : QMainWindow(parent)
    , d(new RibbonWindowPrivate(this))
{
    d->init();
    if (useRibbon) {
        setRibbonTheme(ribbonTheme());
        setMenuWidget(new RibbonBar(this));
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

bool RibbonWindow::isFrameless() const
{
    return d->m_frameless;
}

void RibbonWindow::setFrameless(bool frameless)
{
    if (d->m_frameless == frameless) {
        return;
    }
    d->m_frameless = frameless;
    d->setFrameless(frameless);
    d->m_ribbonBar->setWindowTitleVisible(frameless);
    d->resizeRibbon();
    show();
}

void RibbonWindow::updateWindowFlag(Qt::WindowFlags flags)
{
    if (isUseRibbon() && isFrameless()) {
        if (d->m_windowButtonGroup) {
            d->m_windowButtonGroup->updateWindowFlag(flags);
        }
    }
    repaint();
}

Qt::WindowFlags RibbonWindow::windowButtonFlags() const
{
    if (isUseRibbon() && isFrameless()) {
        if (d->m_windowButtonGroup) {
            return d->m_windowButtonGroup->windowButtonFlags();
        }
    }
    return windowFlags();
}

void RibbonWindow::setMenuWidget(QWidget *menuBar)
{
    QMainWindow::setMenuWidget(menuBar);
    d->setMenuWidget(menuBar);
}

void RibbonWindow::setMenuBar(QMenuBar *menuBar)
{
    QMainWindow::setMenuBar(menuBar);
    d->setMenuWidget(menuBar);
}

void RibbonWindow::resizeEvent(QResizeEvent *event)
{
    if (d->m_ribbonBar) {
        if (d->m_ribbonBar->size().width() != this->size().width()) {
            d->m_ribbonBar->setFixedWidth(this->size().width());
        }
        if (d->m_windowButtonGroup) {
            d->m_ribbonBar->setWindowButtonsSize(d->m_windowButtonGroup->size());
        } else {
            d->m_ribbonBar->setWindowButtonsSize(QSize(0, 0));
        }
    }
    QMainWindow::resizeEvent(event);
}

bool RibbonWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (isFrameless()) {
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
    }
    return QMainWindow::eventFilter(obj, e);
}

bool RibbonWindow::event(QEvent *e)
{
    if (e) {
        switch (e->type()) {
        case QEvent::WindowStateChange: {
            if (isUseRibbon() && isFrameless()) {
                if (d->m_windowButtonGroup) {
                    d->m_windowButtonGroup->setWindowStates(windowState());
                }
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
