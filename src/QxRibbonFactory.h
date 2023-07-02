/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonStyleOption.h"

#include <QScopedPointer>

class RibbonControlButton;

class QX_RIBBON_EXPORT RibbonFactory
{
public:
    RibbonFactory();
    virtual ~RibbonFactory();

    virtual RibbonControlButton *createHideGroupButton(QWidget *parent);

    RibbonStyleOption &getRibbonStyleOption();
    void setRibbonStyleOption(RibbonStyleOption *opt);

private:
    QScopedPointer<RibbonStyleOption> m_opt;
};
