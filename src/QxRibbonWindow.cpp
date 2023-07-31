/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonWindow.h"
#include "FramelessHelper.h"
#include "QxRibbonBar.h"
#include "QxWindowButtonGroup.h"

#include <QApplication>
#include <QFile>
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
    int m_theme;
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
    if (m_useRibbon && frameless) {
        if (Q_NULLPTR == m_framelessHelper) {
            m_framelessHelper = new FramelessHelper(q);
        }
        m_framelessHelper->setTitleHeight(m_ribbonBar->titleBarHeight());
        if (Q_NULLPTR == m_windowButtonGroup) {
            m_windowButtonGroup = new WindowButtonGroup(q);
        }
        m_windowButtonGroup->setWindowStates(q->windowState());
        q->setWindowFlags(q->windowFlags() | Qt::FramelessWindowHint);
    } else {
        destroyFrameless();
        q->setWindowFlags(q->windowFlags() & ~Qt::FramelessWindowHint);
    }
}

void RibbonWindowPrivate::resizeRibbon()
{
    if (m_ribbonBar == Q_NULLPTR) {
        return;
    }
    if (m_windowButtonGroup) {
        m_ribbonBar->setWindowButtonsSize(m_windowButtonGroup->size());
    } else {
        m_ribbonBar->setWindowButtonsSize(QSize(0, 0));
    }
    m_ribbonBar->resizeRibbon();
}

RibbonWindow::RibbonWindow(QWidget *parent, bool useRibbon)
    : QMainWindow(parent)
    , d(new RibbonWindowPrivate(this))
{
    d->init();
    if (useRibbon) {
        setRibbonTheme(ribbonTheme());
        setMenuWidget(new RibbonBar(this));
        d->resizeRibbon();
    }
}

RibbonWindow::~RibbonWindow()
{
    delete d;
}

RibbonBar *RibbonWindow::ribbonBar() const
{
    return d->m_ribbonBar;
}

void RibbonWindow::setRibbonTheme(int theme)
{
    switch (theme) {
    case Office2013Theme:
        loadTheme(":/theme/res/office2013.css");
        break;
    case WpsdarkTheme:
        loadTheme(":/theme/res/wpsdark.css");
        break;
    case NormalTheme:
        loadTheme(":/theme/res/default.css");
        break;
    default:
        break;
    }
    d->m_theme = theme;

    if (d->m_ribbonBar) {
        d->m_ribbonBar->updateRibbonTheme();
    }
}

int RibbonWindow::ribbonTheme() const
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

void RibbonWindow::updateWindowFlags(Qt::WindowFlags flags)
{
    setWindowFlags(flags);
    if (isUseRibbon() && isFrameless()) {
        if (d->m_windowButtonGroup) {
            d->m_windowButtonGroup->updateWindowFlags(flags);
            // FIXME: In the WpsLiteStyle style, there is a short time overlap between rightButtonGroup and
            // windowButtonGroup. Because the button group will be displayed first, and then resize the ribbon.
            // I tried to hide the button group first and display the button group after resize ribbon,
            // but because resize ribbon is a trigger event and non-blocking, the button group was displayed
            // before resize was completed, so I failed.
            d->resizeRibbon();
        }
    }
    show();
}

#if QX_RIBBON_DEPRECATED_SINCE(0, 6)
void RibbonWindow::updateWindowFlag(Qt::WindowFlags flags)
{
    updateWindowFlags(flags);
}

Qt::WindowFlags RibbonWindow::windowButtonFlags() const
{
    return windowFlags();
}
#endif

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
    setStyleSheet(QString::fromUtf8(file.readAll()));
    file.close();
}
