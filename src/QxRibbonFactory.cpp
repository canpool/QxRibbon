/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonFactory.h"
#include "QxRibbonControls.h"

#include <QApplication>
#include <QFontMetrics>

RibbonFactory::RibbonFactory() : m_opt(new RibbonStyleOption())
{
}

RibbonFactory::~RibbonFactory()
{
}

RibbonControlButton *RibbonFactory::createHideGroupButton(RibbonBar *parent)
{
    RibbonControlButton *btn = new RibbonControlButton(parent);

    btn->setAutoRaise(false);
    btn->setObjectName(QStringLiteral("RibbonBarHideGroupButton"));
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setFixedSize(parent->tabBarHeight() - 1, parent->tabBarHeight() - 1);
    return (btn);
}

RibbonStyleOption &RibbonFactory::getRibbonStyleOption()
{
    return (*m_opt);
}

void RibbonFactory::setRibbonStyleOption(RibbonStyleOption *opt)
{
    m_opt.reset(opt);
}
