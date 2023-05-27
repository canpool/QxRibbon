﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonManager.h"

RibbonManager::RibbonManager()
    : m_factory(Q_NULLPTR)
{
    m_factory = new RibbonFactory();
}

RibbonManager::~RibbonManager()
{
    if (m_factory) {
        delete m_factory;
    }
    m_factory = Q_NULLPTR;
}

RibbonManager *RibbonManager::instance()
{
    static RibbonManager s_instance;

    return (&(s_instance));
}

RibbonFactory *RibbonManager::factory()
{
    return (m_factory);
}

void RibbonManager::setupFactory(RibbonFactory *factory)
{
    if (m_factory) {
        delete m_factory;
    }
    m_factory = factory;
}

QX_BEGIN_NAMESPACE

#define QX_VERSION_RIBBON_STR QX_QUOTE(QX_VERSION_RIBBON)

const char *getRibbonVersion(void)
{
    return QX_VERSION_RIBBON_STR;
}

QX_END_NAMESPACE