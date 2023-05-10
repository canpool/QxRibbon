/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonGalleryGroup.h"

#include <QFrame>
#include <QSizeGrip>

class QLabel;
class QVBoxLayout;

class RibbonGalleryPrivate;
class RibbonGalleryViewport;

/**
 * @brief Gallery 控件
 *
 * Gallery 控件是由一个当前激活的 @sa RibbonGalleryGroup 和弹出的 @sa RibbonGalleryViewport 组成
 *
 * @sa RibbonGalleryGroup 是继承 @sa QListView actions 通过 icon 展示出来，相关的属性可以按照 QListView 设置
 *
 * @sa RibbonGalleryViewport 是一个内部有垂直布局的窗体，在弹出激活时，把管理的 RibbonGalleryGroup 都展示出来
 *
 * 示例如下：
 * @code
 * RibbonGallery *gallery = ribbonGroup1->addGallery();
 * QList< QAction* > galleryActions;
 * ...create many actions ...
 * RibbonGalleryGroup *group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
 * galleryActions.clear();
 * ...create many actions ...
 * gallery->addCategoryActions(tr("Apps"), galleryActions);
 * gallery->setCurrentViewGroup(group1);
 * @endcode
 */
class QX_RIBBON_EXPORT RibbonGallery : public QFrame
{
    Q_OBJECT
public:
    RibbonGallery(QWidget *parent = 0);
    virtual ~RibbonGallery();

    RibbonGalleryGroup *addGalleryGroup();
    void addGalleryGroup(RibbonGalleryGroup *group);

    RibbonGalleryGroup *addCategoryActions(const QString &title, QList<QAction *> actions);

    RibbonGalleryGroup *currentViewGroup() const;
    void setCurrentViewGroup(RibbonGalleryGroup *group);

    RibbonGalleryViewport *getPopupViewPort() const;

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

public:
    // 设置最右边三个控制按钮的最大宽度（默认15）
    static void setGalleryButtonMaximumWidth(int w);
signals:
    /**
     * @brief 转发管理的RibbonGalleryGroup::triggered
     * 所有加入RibbonGallery的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction *action);
    /**
     * @brief 转发管理的RibbonGalleryGroup::hovered
     * @note 此属性需要确保RibbonGalleryGroup::setMouseTracking(true)
     * @param action
     */
    void hovered(QAction *action);

public slots:
    virtual void pageUp();
    virtual void pageDown();
    virtual void showMoreDetail();
protected slots:
    void onItemClicked(const QModelIndex &index);
    virtual void onTriggered(QAction *action);
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    RibbonGalleryViewport *ensureGetPopupViewPort();
private:
    RibbonGalleryPrivate *d;
    friend class RibbonGalleryPrivate;
};

class RibbonGalleryViewportPrivate;
/* RibbonGalleryViewport */
class QX_RIBBON_EXPORT RibbonGalleryViewport : public QWidget
{
    Q_OBJECT
public:
    RibbonGalleryViewport(QWidget *parent);
    ~RibbonGalleryViewport();

    void addWidget(QWidget *w);
    void addWidget(QWidget *w, const QString &title);

    void removeWidget(QWidget *w);

    QLabel *getWidgetTitleLabel(QWidget *w);
public slots:
    void widgetTitleChanged(QWidget *w, const QString &title);
private:
    RibbonGalleryViewportPrivate *d;
};
