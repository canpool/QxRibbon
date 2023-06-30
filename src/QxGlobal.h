/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include <QtCore/qglobal.h>

#ifndef Q_NULLPTR
#define Q_NULLPTR 0
#endif

#ifndef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE
#endif

#define QX_VERSION_JOIN2(major, minor, patch) major##.##minor##.##patch
#define QX_VERSION_JOIN(major, minor, patch) QX_VERSION_JOIN2(major, minor, patch)

#define QX_DECLARE_PRIVATE(Class) \
    Class##Private *d_ptr; \
    Q_DECLARE_PRIVATE(Class)

#define QX_DECLARE_PUBLIC(Class) \
    Class *q_ptr; \
    inline void setPublic(Class *ptr) { q_ptr = ptr; } \
    Q_DECLARE_PUBLIC(Class)

#define QX_INIT_PRIVATE(Class) \
    d_ptr = new Class##Private(); \
    d_ptr->setPublic(this);

#define QX_FINI_PRIVATE() \
    delete d_ptr; d_ptr = Q_NULLPTR;

#if !defined(QX_NAMESPACE_DISABLE)
#define QX_NAMESPACE  Qx
#endif

#if !defined(QX_NAMESPACE)
#define QX_BEGIN_NAMESPACE
#define QX_END_NAMESPACE
#define QX_USE_NAMESPACE
#define QX_PREPEND_NAMESPACE(name) name
#else
#define QX_BEGIN_NAMESPACE namespace QX_NAMESPACE {
#define QX_END_NAMESPACE }
#define QX_USE_NAMESPACE using namespace QX_NAMESPACE;
#define QX_PREPEND_NAMESPACE(name) QX_NAMESPACE::name
#endif // QX_NAMESPACE

QX_BEGIN_NAMESPACE
// nothing
QX_END_NAMESPACE
