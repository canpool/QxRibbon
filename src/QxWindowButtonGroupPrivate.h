/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "QxRibbonGlobal.h"
#include <QPushButton>

class WindowButtonGroup;

class WindowToolButton : public QPushButton
{
    Q_OBJECT
public:
    explicit WindowToolButton(QWidget *p = Q_NULLPTR);
};

class WindowButtonGroupPrivate : public QObject
{
    Q_OBJECT
    QX_DECLARE_PUBLIC(WindowButtonGroup)
public:
    WindowButtonGroupPrivate();

    void setupMinimizeButton(bool on);
    void setupMaximizeButton(bool on);
    void setupCloseButton(bool on);
    void updateSize();
    void resize(QSize size);
    QSize sizeHint() const;
    int groupHeight() const;
public slots:
    void buttonClicked();
public:
    WindowToolButton *m_closeButton;
    WindowToolButton *m_minimizeButton;
    WindowToolButton *m_maximizeButton;
    int m_closeStretch;
    int m_maxStretch;
    int m_minStretch;
    qreal m_iconScale;
    bool m_signalEnabled;
};
