/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonGroup.h"

#include <QToolButton>

class RibbonButton;
class RibbonGroupItem;
class RibbonGroupLayout;

/**
 * @brief Group右下角的操作按钮
 *
 * 此按钮和一个action关联，使用@ref RibbonGroup::addOptionAction 函数用于生成此按钮，正常来说
 * 用户并不需要直接操作此类，仅仅用于样式设计
 * 如果一定要重载此按钮，可以通过重载@ref RibbonFactory
 * 的 @ref RibbonFactory::createRibbonGroupOptionButton来实现新的OptionButton
 *
 */
class RibbonGroupOptionButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupOptionButton(QWidget *parent = Q_NULLPTR);
    // 有别于setDefaultAction 此函数只关联action的响应，而不设置text，icon等
    void connectAction(QAction *action);
};

class RibbonGroupPrivate
{
public:
    RibbonGroupPrivate(RibbonGroup *p);

    void resetLargeToolButtonStyle();

    // 返回最后一个添加的action对应的button，前提是最后一个是toolbutton，否则返回Q_NULLPTR
    RibbonButton *lastAddActionButton();

    const QList<RibbonGroupItem *> &ribbonGroupItems() const;
public:
    RibbonGroup *q;
    RibbonGroupLayout *m_layout;
    RibbonGroupOptionButton *m_optionActionButton;      ///< 标题栏的y距离
    RibbonGroup::GroupLayoutMode m_groupLayoutMode;   ///< group的布局模式，默认为3行模式ThreeRowMode
    static int s_groupTitleHeight;                       ///< group标题栏的全局高度,默认为21
    bool m_isCanCustomize;                                ///< 记录是否可自定义
};
