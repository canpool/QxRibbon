﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonBar.h"

#include <QStackedWidget>

class RibbonControlButton;

class RibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit RibbonStackedWidget(QWidget *parent = Q_NULLPTR);
    virtual ~RibbonStackedWidget();

    bool isPopup() const;
    void setPopup(bool popup);

    void moveWidget(int from, int to);

signals:
    void aboutToShow();
    void aboutToHide();

protected:
    virtual bool event(QEvent *event);

private:
    bool m_isPopup;
};

class PageContextManagerData
{
public:
    RibbonPageContext *pageContext;
    QList<int> tabPageIndex;
    bool operator==(const RibbonPageContext *pageContext)
    {
        return (this->pageContext == pageContext);
    }
};

class RibbonTabData
{
public:
    RibbonPage *page;
    int index;
    RibbonTabData() : page(Q_NULLPTR), index(-1)
    {
    }
};
Q_DECLARE_METATYPE(RibbonTabData)

class RibbonBarPrivate : public QObject
{
    Q_OBJECT
public:
    RibbonBarPrivate(RibbonBar *par);

    void init();

    void setApplicationButton(QAbstractButton *btn);
    bool isContainPageContextInList(RibbonPageContext *pageContext);

    void setMinimizedFlag(bool flag);
    void updateMinimumButtonIcon();

    QColor getPageContextColor();

    bool isOfficeStyle() { return ((m_ribbonStyle & 0x00FF) == 0); }
    bool isTwoRowStyle() { return ((m_ribbonStyle & 0xFF00) != 0); }

    void updateTabData();
    void updatePageContextManagerData();
    void updateRibbonElementGeometry();
    void updateRibbonBarHeight();

    void paintInOfficeStyle(QPainter &p);
    void paintInWpsLiteStyle(QPainter &p);
    void paintBackground(QPainter &painter);
    void paintWindowTitle(QPainter &painter, const QString &title, const QRect &titleRegion);
    void paintPageContextTab(QPainter &painter, const QString &title, QRect contextRect, const QColor &color);

    void resizeInOfficeStyle();
    void resizeInWpsLiteStyle();
    void resizeStackedWidget();
    void resizeRibbon();

    int calcMinTabBarWidth() const;
    int mainBarHeight() const;

    int tabIndex(RibbonPage *obj);

public slots:
    void onWindowTitleChanged(const QString &title);
    void onWindowIconChanged(const QIcon &icon);
    void onPageWindowTitleChanged(const QString &title);
    void onPageContextAdded(RibbonPage *page);
    void onCurrentRibbonTabChanged(int index);
    void onCurrentRibbonTabClicked(int index);
    void onCurrentRibbonTabDoubleClicked(int index);
    void onTabMoved(int from, int to);
    void onStackWidgetHided();
public:
    RibbonBar *q;
    QAbstractButton *m_applicationButton;
    RibbonTabBar *m_tabBar;
    RibbonStackedWidget *m_stack;
    QList<PageContextManagerData> m_currentShowingPageContextList;
    QList<RibbonPageContext *> m_pageContextList;
    QList<RibbonTabData> m_hidedPage;
    int m_iconRightBorderPosition;   ///< 标题栏x值的最小值，在有图标和快捷启动按钮，此值都需要变化
    RibbonControlButton *m_minimumPageButton;   ///< 隐藏面板按钮
    RibbonButtonGroup *m_rightButtonGroup;      ///< 在 tabbar 右边的按钮群
    RibbonQuickAccessBar *m_quickAccessBar;
    RibbonBar::QuickAccessBarPosition m_quickAccessBarPosition;
    RibbonBar::RibbonStyle m_ribbonStyle;
    RibbonBar::RibbonStyle m_lastShowStyle;
    QSize m_windowButtonsSize;  ///< 由 RibbonWindow 告诉的 windowbuttons 的尺寸
    QList<QColor> m_pageContextColorList;
    int m_pageContextColorListIndex;
    QColor m_tabBarBaseLineColor;
    Qt::Alignment m_titleAligment;
    bool m_minimized;
    bool m_titleVisible;
    bool m_pageContextCoverTab;
    bool m_layoutRequest;
};
