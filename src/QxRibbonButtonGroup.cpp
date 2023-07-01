/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonButtonGroup.h"
#include "QxRibbonControls.h"
#include "QxRibbonButton.h"

#include <QActionEvent>
#include <QHBoxLayout>
#include <QWidgetAction>

/* RibbonButtonGroupItem */
class RibbonButtonGroupItem
{
public:
    RibbonButtonGroupItem();
    RibbonButtonGroupItem(QAction *a, QWidget *w, bool cw);

    bool operator==(QAction *action);
    bool operator==(const RibbonButtonGroupItem &w);
public:
    QAction *action;
    QWidget *widget;
    bool customWidget;
};

RibbonButtonGroupItem::RibbonButtonGroupItem()
    : action(Q_NULLPTR)
    , widget(Q_NULLPTR)
    , customWidget(false)
{
}

RibbonButtonGroupItem::RibbonButtonGroupItem(QAction *a, QWidget *w, bool cw)
    : action(a)
    , widget(w)
    , customWidget(cw)
{
}

bool RibbonButtonGroupItem::operator==(QAction *action)
{
    return (this->action == action);
}

bool RibbonButtonGroupItem::operator==(const RibbonButtonGroupItem &w)
{
    return (this->action == w.action);
}

/* RibbonButtonGroupPrivate */
class RibbonButtonGroupPrivate
{
public:
    RibbonButtonGroupPrivate(RibbonButtonGroup *p);

    void init();
public:
    RibbonButtonGroup *q;
    QList<RibbonButtonGroupItem> m_items;
};

RibbonButtonGroupPrivate::RibbonButtonGroupPrivate(RibbonButtonGroup *p)
    : q(p)
{
}

void RibbonButtonGroupPrivate::init()
{
    QHBoxLayout *layout = new QHBoxLayout(q);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    q->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

/* RibbonButtonGroup */
RibbonButtonGroup::RibbonButtonGroup(QWidget *parent)
    : QFrame(parent)
    , d(new RibbonButtonGroupPrivate(this))
{
    d->init();
}

RibbonButtonGroup::~RibbonButtonGroup()
{
    for (RibbonButtonGroupItem &item : d->m_items) {
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(item.action)) {
            if (item.customWidget) {
                widgetAction->releaseWidget(item.widget);
            }
        }
    }
    delete d;
}

QAction *RibbonButtonGroup::addAction(QAction *a)
{
    QWidget::addAction(a);
    return a;
}

QAction *RibbonButtonGroup::addAction(const QString &text, const QIcon &icon,
                                      QToolButton::ToolButtonPopupMode popMode)
{
    QAction *a = new QAction(icon, text, this);

    addAction(a);
    RibbonButton *btn = qobject_cast<RibbonButton *>(d->m_items.last().widget);
    if (btn) {
        btn->setPopupMode(popMode);
    }
    return a;
}

QAction *RibbonButtonGroup::addMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode)
{
    QAction *a = menu->menuAction();

    addAction(a);
    RibbonButton *btn = qobject_cast<RibbonButton *>(d->m_items.last().widget);
    if (btn) {
        btn->setPopupMode(popMode);
    }
    return a;
}

QAction *RibbonButtonGroup::addSeparator()
{
    QAction *a = new QAction(this);

    a->setSeparator(true);
    addAction(a);
    return (a);
}

QAction *RibbonButtonGroup::addWidget(QWidget *w)
{
    QWidgetAction *a = new QWidgetAction(this);

    a->setDefaultWidget(w);
    w->setAttribute(Qt::WA_Hover);
    addAction(a);
    return a;
}

QSize RibbonButtonGroup::sizeHint() const
{
    return layout()->sizeHint();
}

QSize RibbonButtonGroup::minimumSizeHint() const
{
    return layout()->minimumSize();
}

void RibbonButtonGroup::actionEvent(QActionEvent *e)
{
    RibbonButtonGroupItem item;
    item.action = e->action();

    switch (e->type()) {
    case QEvent::ActionAdded: {
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(item.action)) {
            widgetAction->setParent(this);
            item.widget = widgetAction->requestWidget(this);
            if (item.widget != Q_NULLPTR) {
                item.widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
                item.widget->show();
                item.customWidget = true;
            }
        } else if (item.action->isSeparator()) {
            RibbonSeparator *sp = new RibbonSeparator(this);
            sp->setTopBottomMargins(3, 3);
            item.widget = sp;
        }
        if (!item.widget) {
            RibbonButton *button = new RibbonButton(this);
            button->setAutoRaise(true);
            button->setFocusPolicy(Qt::NoFocus);
            button->setButtonType(RibbonButton::SmallButton);
            button->setToolButtonStyle(Qt::ToolButtonIconOnly);
            button->setDefaultAction(item.action);
            QObject::connect(button, &RibbonButton::triggered, this, &RibbonButtonGroup::actionTriggered);
            item.widget = button;
        }
        layout()->addWidget(item.widget);
        d->m_items.append(item);
    } break;
    case QEvent::ActionChanged: {
        layout()->invalidate();
    } break;
    case QEvent::ActionRemoved: {
        item.action->disconnect(this);
        auto i = d->m_items.begin();
        for (; i != d->m_items.end();) {
            QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(i->action);
            if ((widgetAction != 0) && i->customWidget) {
                widgetAction->releaseWidget(i->widget);
            } else {
                // destroy the QToolButton/QToolBarSeparator
                i->widget->hide();
                i->widget->deleteLater();
            }
            i = d->m_items.erase(i);
        }
        layout()->invalidate();
    } break;
    default:
        break;
    }
}
