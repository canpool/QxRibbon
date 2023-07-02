/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonQuickAccessBar.h"
#include "QxRibbonButtonGroup.h"

class RibbonQuickAccessBarPrivate
{
public:
    RibbonButtonGroup *m_buttonGroup;
};

RibbonQuickAccessBar::RibbonQuickAccessBar(QWidget *parent)
    : RibbonCtrlContainer(parent)
    , d(new RibbonQuickAccessBarPrivate)
{
    d->m_buttonGroup = new RibbonButtonGroup(this);
    d->m_buttonGroup->setObjectName(QStringLiteral("qx_RibbonButtonGroup"));
    setWidget(d->m_buttonGroup);
    setTitleVisible(false);
}

RibbonQuickAccessBar::~RibbonQuickAccessBar()
{
    delete d;
}

void RibbonQuickAccessBar::addSeparator()
{
    d->m_buttonGroup->addSeparator();
}

void RibbonQuickAccessBar::addAction(QAction *act)
{
    d->m_buttonGroup->addAction(act);
}

void RibbonQuickAccessBar::insertAction(QAction *before, QAction *action)
{
    d->m_buttonGroup->insertAction(before, action);
}

void RibbonQuickAccessBar::insertActions(QAction *before, QList<QAction *> actions)
{
    d->m_buttonGroup->insertActions(before, actions);
}

void RibbonQuickAccessBar::removeAction(QAction *action)
{
    d->m_buttonGroup->removeAction(action);
}

void RibbonQuickAccessBar::addWidget(QWidget *w)
{
    d->m_buttonGroup->addWidget(w);
}

void RibbonQuickAccessBar::addMenu(QMenu *m, QToolButton::ToolButtonPopupMode popMode)
{
    d->m_buttonGroup->addMenu(m, popMode);
}
