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

#ifdef Q_OS_WINDOWS
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <QScreen>
#endif

class RibbonWindowPrivate
{
public:
    RibbonWindowPrivate(RibbonWindow *p);
    void init();
    void setMenuWidget(QWidget *menuBar);
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

void RibbonWindowPrivate::setMenuWidget(QWidget *menuBar)
{
    RibbonBar *bar = qobject_cast<RibbonBar *>(menuBar);

    if (bar) {
        m_ribbonBar = bar;
        m_ribbonBar->installEventFilter(q);
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
        m_useRibbon = true;
        m_windowButtonGroup->show();
    } else {
        m_ribbonBar = Q_NULLPTR;
        m_useRibbon = false;
        if (m_framelessHelper) {
            delete m_framelessHelper;
            m_framelessHelper = Q_NULLPTR;
        }
        if (m_windowButtonGroup) {
            m_windowButtonGroup->hide();
        }
    }
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

#ifdef Q_OS_WINDOWS
bool RibbonWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (isUseRibbon()) {
        MSG *msg = reinterpret_cast<MSG *>(message);
        switch (msg->message) {
            case WM_GETMINMAXINFO: {
                // prevent taskbar is covered when maximized
                if (this->isMaximized()) {
                    const QRect rc = this->screen()->availableGeometry();
                    MINMAXINFO *p = (MINMAXINFO *)(msg->lParam);
                    p->ptMaxPosition.x = 0;
                    p->ptMaxPosition.y = 0;
                    p->ptMaxSize.x = rc.width();
                    p->ptMaxSize.y = rc.height();
                    *result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
                    return true;
                }
            }
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}
#endif // Q_OS_WINDOWS
