﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGroup.h"

#include <QObject>
#include <QToolButton>

class RibbonPage;
class RibbonSeparator;
class RibbonBar;
class RibbonPageScrollButton;

class RibbonPageItem
{
public:
    RibbonPageItem() : m_group(Q_NULLPTR), m_separator(Q_NULLPTR)
    {
    }

    bool isEmpty() const
    {
        if (m_group) {
            return m_group->isHidden();
        }
        return true;
    }

    bool isNull() const
    {
        return (m_group == Q_NULLPTR);
    }
public:
    RibbonGroup *m_group;
    RibbonSeparator *m_separator;
    QRect m_willSetGroupGeometry;
    QRect m_willSetSeparatorGeometry;
};


class RibbonPageScrollButton : public QToolButton
{
    Q_OBJECT
public:
    RibbonPageScrollButton(Qt::ArrowType arr, QWidget *p = Q_NULLPTR);
};

class RibbonPagePrivate : public QObject
{
    Q_OBJECT
public:
    RibbonPagePrivate(RibbonPage *parent);
    virtual ~RibbonPagePrivate();

    void init();

    RibbonGroup *addGroup(const QString &title);
    void addGroup(RibbonGroup *group);
    RibbonGroup *insertGroup(int index, const QString &title);
    void insertGroup(int index, RibbonGroup *group);

    bool takeGroup(RibbonGroup *group);

    bool removeGroup(RibbonGroup *group);

    QList<RibbonGroup *> groupList();

    void setGroupLayoutMode(RibbonGroup::GroupLayoutMode m);

    void setBackgroundBrush(const QBrush &brush);

    int totalSizeHintWidth() const;

    QSize pageContentSize() const;

    void updateItemGeometry();

    void doItemLayout();

    void doWheelEvent(QWheelEvent *event);

public slots:
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();
public:
    RibbonPage *q;
    RibbonBar *m_ribbonBar;
    RibbonGroup::GroupLayoutMode m_groupLayoutMode;
    RibbonPageScrollButton *m_leftScrollBtn;
    RibbonPageScrollButton *m_rightScrollBtn;
    QList<RibbonPageItem> m_itemList;
    QSize m_sizeHint;
    QMargins m_contentsMargins;
    int m_totalWidth;
    int m_XBase;
    bool m_isRightScrollBtnShow;
    bool m_isLeftScrollBtnShow;
    bool m_isPageContext;
    bool m_isCanCustomize;
};
