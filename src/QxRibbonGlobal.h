/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxGlobal.h"

#ifndef QX_RIBBON_LIBRARY_STATIC
#ifdef  QX_RIBBON_LIBRARY
#define QX_RIBBON_EXPORT Q_DECL_EXPORT
#else
#define QX_RIBBON_EXPORT Q_DECL_IMPORT
#endif // QX_RIBBON_LIBRARY
#else  // QX_RIBBON_LIBRARY_STATIC
#define QX_RIBBON_EXPORT
#endif // QX_RIBBON_LIBRARY_STATIC
