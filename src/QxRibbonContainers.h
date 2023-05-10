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
    QIcon getIcon() const;

    void setText(const QString &t);
    QString getText() const;

    bool hasWidget() const;
    void setWidget(QWidget *w);
    QWidget *getWidget();
    const QWidget *getWidget() const;
private:
    RibbonCtrlContainerPrivate *d;
};
