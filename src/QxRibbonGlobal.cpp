/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonGlobal.h"

QX_BEGIN_NAMESPACE

#define QX_VERSION_RIBBON_STR QX_QUOTE(QX_VERSION_RIBBON)

const char *getRibbonVersion(void)
{
    return QX_VERSION_RIBBON_STR;
}

QX_END_NAMESPACE
