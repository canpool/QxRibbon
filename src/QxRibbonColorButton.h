/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonButton.h"

class RibbonColorButtonPrivate;

class QX_RIBBON_EXPORT RibbonColorButton : public RibbonButton
{
    Q_OBJECT
public:
    explicit RibbonColorButton(QWidget *parent = nullptr);
    explicit RibbonColorButton(QAction *defaultAction, QWidget *parent = nullptr);
    virtual ~RibbonColorButton();

    const QColor &color() const;
signals:
    void colorChanged(const QColor &color);
    void colorClicked(const QColor &color, bool checked = false);
public slots:
    void setColor(const QColor &color);
private slots:
    void onButtonClicked(bool checked = false);
private:
    RibbonColorButtonPrivate *dd;
};
