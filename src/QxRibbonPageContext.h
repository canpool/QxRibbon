﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonPage.h"

#include <QWidget>

class RibbonPageContextPrivate;

/**
 * @brief 管理上下文标签的类
 */
class QX_RIBBON_EXPORT RibbonPageContext : public QObject
{
    Q_OBJECT
public:
    RibbonPageContext(QWidget *parent = Q_NULLPTR);
    ~RibbonPageContext();

    RibbonPage *addPage(const QString &title);
    RibbonPage *page(int index);

    int pageCount() const;

    QList<RibbonPage *> pageList() const;

    bool takePage(RibbonPage *page);

    QVariant id() const;
    void setId(const QVariant &id);

    QColor contextColor() const;
    void setContextColor(const QColor color);

    QString contextTitle() const;
    void setContextTitle(const QString &title);

signals:
    void pageAdded(RibbonPage *page);
    void pageRemoved(RibbonPage *page);
protected:
    QWidget *parentWidget() const;
    virtual bool eventFilter(QObject *watched, QEvent *e) override;
private:
    RibbonPageContextPrivate *d;
};