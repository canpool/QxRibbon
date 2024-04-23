/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonTheme.h"

#include <QApplication>
#include <QWidget>
#include <QFile>

void RibbonTheme::loadTheme(const QString &themeFile, QWidget *widget)
{
    QFile file(themeFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString css = QString::fromUtf8(file.readAll());
    if (widget) {
        widget->setStyleSheet(css);
    } else {
        qApp->setStyleSheet(css);
    }

    file.close();
}

void RibbonTheme::setTheme(int themeStyle, QWidget *widget)
{
    switch (themeStyle) {
    case Office2013Theme:
        loadTheme(":/theme/res/office2013.css", widget);
        break;
    case WpsdarkTheme:
        loadTheme(":/theme/res/wpsdark.css", widget);
        break;
    case NormalTheme:
        loadTheme(":/theme/res/default.css", widget);
        break;
    default:
        break;
    }
}
