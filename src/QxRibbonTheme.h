/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include <QObject>

class QWidget;

class QX_RIBBON_EXPORT RibbonTheme : public QObject
{
    Q_OBJECT
public:
    enum ThemeStyle {
        NormalTheme,
        Office2013Theme,
        WpsdarkTheme,
        CustomTheme,
    };
    Q_ENUM(ThemeStyle)
public:
    /** If the widget is nullptr, css/qss will be applied to QApplication */
    static void loadTheme(const QString &themeFile, QWidget *widget = nullptr);

    /** If you have a custom theme, please use loadTheme */
    static void setTheme(int themeStyle, QWidget *widget = nullptr);
};
