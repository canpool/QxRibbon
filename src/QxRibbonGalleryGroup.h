/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include <QListView>

class QActionGroup;

class RibbonGalleryGroupPrivate;

///
/// \brief Gallery 的组
///
/// 组负责显示管理 Gallery Item
///
class QX_RIBBON_EXPORT RibbonGalleryGroup : public QListView
{
    Q_OBJECT
public:
    /**
     * @brief GalleryGroup显示的样式
     */
    enum GalleryGroupStyle {
        IconWithText,           ///< 图标带文字
        IconWithWordWrapText,   ///< 图标带文字,文字会换行显示，此模式只会对DisplayOneRow生效，如果不是DisplayOneRow，等同IconWithText
        IconOnly                ///< 只有图标
    };

    /**
     * @brief 定义Gallery在一个group下面显示的图标行数
     */
    enum DisplayRow {
        DisplayOneRow = 1,   ///< 显示1行，默认
        DisplayTwoRow = 2,
        DisplayThreeRow = 3
    };
public:
    explicit RibbonGalleryGroup(QWidget *w = Q_NULLPTR);
    virtual ~RibbonGalleryGroup();

    // 重新计算grid尺寸
    void setRecalcGridSizeBlock(bool on = true);
    bool isRecalcGridSizeBlock() const;
    void recalcGridSize();
    void recalcGridSize(int galleryHeight);
    // 设置显示的样式
    void setGalleryGroupStyle(GalleryGroupStyle style);
    GalleryGroupStyle getGalleryGroupStyle() const;
    // 添加一个item
    void addItem(const QString &text, const QIcon &icon);
    // 以一个aciton作为item添加
    void addActionItem(QAction *act);
    void addActionItemList(const QList<QAction *> &acts);
    // 标题
    void setGroupTitle(const QString &title);
    QString getGroupTitle() const;
    void selectByIndex(int i);
    // 设置显示的行数
    void setDisplayRow(DisplayRow r);
    DisplayRow getDisplayRow() const;
    // 设置grid最小的宽度，默认为0（不限制），可以限定grid的宽度
    void setGridMinimumWidth(int w);
    int getGridMinimumWidth() const;
    // 设置grid最大的宽度，默认为0（不限制），可以限定grid的宽度
    void setGridMaximumWidth(int w);
    int getGridMaximumWidth() const;
    // 获取RibbonGalleryGroup管理的actiongroup
    QActionGroup *getActionGroup() const;
private slots:
    void onItemClicked(const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);
signals:
    void groupTitleChanged(const QString &title);
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入RibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction *action);
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入RibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @note 此属性需要通过QAbstractItemView::entered(const QModelIndex &index)激活，因此要保证设置了setMouseTracking(true)
     * @param action
     */
    void hovered(QAction *action);
private:
    RibbonGalleryGroupPrivate *d;
};
