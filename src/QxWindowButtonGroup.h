﻿/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include <QWidget>

class WindowButtonGroupPrivate;

class QX_RIBBON_EXPORT WindowButtonGroup : public QWidget
{
    Q_OBJECT
public:
    explicit WindowButtonGroup(QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags());
    ~WindowButtonGroup();

    void updateWindowFlags(Qt::WindowFlags flags);

    void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);
    void setIconScale(qreal iconscale = 0.5);
    void setWindowStates(Qt::WindowStates s);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /** default is disabled */
    bool signalIsEnabled() const;
public Q_SLOTS:
    void setSignalEnabled(bool enable);
Q_SIGNALS:
    void buttonMinimizeClicked();
    void buttonMaximzieClicked();
    void buttonCloseClicked();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *e) Q_DECL_OVERRIDE;
    virtual void parentResize();
    virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
protected slots:
    void buttonClicked();
private:
    friend class WindowButtonGroupPrivate;
    WindowButtonGroupPrivate *d;
};

