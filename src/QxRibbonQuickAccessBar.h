/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include <QToolBar>

#include <QMenu>


class RibbonQuickAccessBarPrivate;

/* RibbonQuickAccessBar */
class QX_RIBBON_EXPORT RibbonQuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    explicit RibbonQuickAccessBar(QWidget *parent = Q_NULLPTR);
    virtual ~RibbonQuickAccessBar();

public:
    QAction *actionCustomizeButton() const;
    void setActionVisible(QAction *action, bool visible);
    bool isActionVisible(QAction *action) const;
    int visibleCount() const;

    QByteArray state() const;
    void setState(const QByteArray &s);

Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);
    void customizeActionChanged();

protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *event);

private:
    QX_DECLARE_PRIVATE(RibbonQuickAccessBar)
};
