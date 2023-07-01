/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include <QWidget>

/**
 * @brief Control Widget Container
 *
 * LeftToRight:
 *  ------------------------
 *  | icon | text | widget |
 *  ------------------------
 */
class RibbonCtrlContainerPrivate;
class QX_RIBBON_EXPORT RibbonCtrlContainer : public QWidget
{
    Q_OBJECT
public:
    RibbonCtrlContainer(QWidget *parent = Q_NULLPTR);
    ~RibbonCtrlContainer();

    void setIconVisible(bool b);
    void setTitleVisible(bool b);

    void setIcon(const QIcon &i);
    QIcon icon() const;

    void setIconSize(const QSize &size);
    QSize iconSize() const;

    void setText(const QString &t);
    QString text() const;

    void setWidget(QWidget *w);
    QWidget *widget() const;
private:
    RibbonCtrlContainerPrivate *d;
};
