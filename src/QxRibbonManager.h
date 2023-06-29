/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonFactory.h"

class QX_RIBBON_EXPORT RibbonManager
{
protected:
    RibbonManager();
public:
    virtual ~RibbonManager();
    static RibbonManager *instance();
    RibbonFactory *factory();
    void setupFactory(RibbonFactory *factory);
private:
    RibbonFactory *m_factory;
};

#ifndef RibbonSubElementMgr
#define RibbonSubElementMgr RibbonManager::instance()
#endif

#ifndef RibbonSubElementFactory
#define RibbonSubElementFactory RibbonManager::instance()->factory()
#endif

#ifndef RibbonSubElementStyleOpt
#define RibbonSubElementStyleOpt RibbonSubElementFactory->getRibbonStyleOption()
#endif

