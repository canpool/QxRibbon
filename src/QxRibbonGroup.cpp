﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonGroup.h"
#include "QxRibbonGroupPrivate.h"
#include "QxRibbonPage.h"
#include "QxRibbonManager.h"
#include "QxRibbonGallery.h"
#include "QxRibbonGroupLayout.h"
#include "QxRibbonControls.h"
#include "QxRibbonButton.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QGridLayout>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QResizeEvent>
#include <QWidgetAction>

#ifndef QX_RIBBON_PROP_ROW_PROPORTION
#define QX_RIBBON_PROP_ROW_PROPORTION "_qx_RibbonGroupRowProportion"
#endif

#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)


RibbonGroupOptionButton::RibbonGroupOptionButton(QWidget *parent) : QToolButton(parent)
{
    setAutoRaise(true);
    setCheckable(false);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setFixedSize(16, 16);
    setIconSize(QSize(10, 10));
    setIcon(QIcon(":/image/res/ribbonGroupOptionButton.png"));
}

void RibbonGroupOptionButton::connectAction(QAction *action)
{
    connect(this, &RibbonGroupOptionButton::clicked, action, &QAction::trigger);
}

int RibbonGroupPrivate::s_groupTitleHeight = 21;

RibbonGroupPrivate::RibbonGroupPrivate(RibbonGroup *p)
    : q(p)
    , m_optionActionButton(Q_NULLPTR)
    , m_groupLayoutMode(RibbonGroup::ThreeRowMode)
    , m_isCanCustomize(true)
{
    m_layout = new RibbonGroupLayout(q);
    m_layout->setSpacing(2);
    m_layout->setContentsMargins(2, 2, 2, 2);
}

RibbonButton *RibbonGroupPrivate::lastAddActionButton()
{
    QWidget *w = m_layout->lastWidget();

    return qobject_cast<RibbonButton *>(w);
}

/**
 * @brief 重置大按钮的类型
 */
void RibbonGroupPrivate::resetLargeToolButtonStyle()
{
    QList<RibbonButton *> btns = q->ribbonButtons();

    for (RibbonButton *b : btns) {
        if ((Q_NULLPTR == b) || (RibbonButton::LargeButton != b->buttonType())) {
            continue;
        }
        if (RibbonGroup::ThreeRowMode == m_groupLayoutMode) {
            if (RibbonButton::Normal != b->largeButtonType()) {
                b->setLargeButtonType(RibbonButton::Normal);
            }
        } else {
            if (RibbonButton::Lite != b->largeButtonType()) {
                b->setLargeButtonType(RibbonButton::Lite);
            }
        }
    }
}

/**
 * @brief 获取布局item
 * @return
 */
const QList<RibbonGroupItem *> &RibbonGroupPrivate::ribbonGroupItems() const
{
    return m_layout->ribbonGroupItems();
}

/* RibbonGroup */
RibbonGroup::RibbonGroup(QWidget *parent)
    : QWidget(parent)
    , d(new RibbonGroupPrivate(this))
{
    setGroupLayoutMode(ThreeRowMode);
}

RibbonGroup::RibbonGroup(const QString &name, QWidget *parent)
    : RibbonGroup(parent)
{
    setGroupName(name);
}

RibbonGroup::~RibbonGroup()
{
    delete d;
}

QString RibbonGroup::groupName() const
{
    return windowTitle();
}

/**
 * @brief RibbonGroup::setGroupTitle group的标题
 * @note 注意会触发windowTitleChange信号
 * @param title 标题
 */
void RibbonGroup::setGroupName(const QString &title)
{
    setWindowTitle(title);
    update();
}

/**
 * @brief 获取action的行属性
 * @param action
 * @return
 */
RibbonGroup::RowProportion RibbonGroup::getActionRowProportionProperty(QAction *action)
{
    bool isok = false;
    int r = action->property(QX_RIBBON_PROP_ROW_PROPORTION).toInt(&isok);

    if (isok) {
        return static_cast<RibbonGroup::RowProportion>(r);
    }
    return RibbonGroup::Large;
}

/**
 * @brief 把action的行属性设置进action中，action自身携带了行属性
 * @param action
 * @param rp
 */
void RibbonGroup::setActionRowProportionProperty(QAction *action, RibbonGroup::RowProportion rp)
{
    if (action == Q_NULLPTR) {
        return;
    }
    action->setProperty(QX_RIBBON_PROP_ROW_PROPORTION, int(rp));
}

/**
 * @brief 设置action的行行为，行属性决定了ribbon group的显示方式
 * @param action 需要设置的action，此action必须已经被group添加过
 * @param rp 行为
 */
void RibbonGroup::setActionRowProportion(QAction *action, RibbonGroup::RowProportion rp)
{
    if (action == Q_NULLPTR) {
        return;
    }
    RibbonGroupLayout *lay = d->m_layout;

    setActionRowProportionProperty(action, rp);
    if (lay) {
        RibbonGroupItem *it = lay->groupItem(action);
        if (it) {
            it->rowProportion = rp;
            lay->invalidate();
        }
    }
}

/**
 * @brief 添加action
 * @param action action
 * @param rp 指定action的行占比
 * @return 返回对应的RibbonButton，如果是窗口，返回的toolbutton为Q_NULLPTR
 */
RibbonButton *RibbonGroup::addAction(QAction *action, RibbonGroup::RowProportion rp)
{
    if (action == Q_NULLPTR) {
        return Q_NULLPTR;
    }
    setActionRowProportionProperty(action, rp);
    QWidget::addAction(action);

    return d->lastAddActionButton();
}

/**
 * @brief 添加大图标
 *
 * @param action
 * @sa 如果想获取actiom对应的RibbonButton,可以使用@ref ribbonButtonForAction 函数
 */
RibbonButton *RibbonGroup::addLargeAction(QAction *action)
{
    return addAction(action, RibbonGroup::Large);
}

/**
 * @brief 在三栏模式下，强制加为2栏action
 * @note 在两行模式下，Medium和Small等价
 * 主要应用在ThreeRowMode下
 * @param action
 * @sa 如果想获取actiom对应的RibbonButton,可以使用@ref ribbonButtonForAction 函数
 */
RibbonButton *RibbonGroup::addMediumAction(QAction *action)
{
    return addAction(action, RibbonGroup::Medium);
}

/**
 * @brief 添加小图标
 * @param action
 * @sa 如果想获取actiom对应的RibbonButton,可以使用@ref ribbonButtonForAction 函数
 */
RibbonButton *RibbonGroup::addSmallAction(QAction *action)
{
    return addAction(action, RibbonGroup::Small);
}

/**
 * @brief 添加一个action
 * @param act
 * @param popMode 按钮的样式
 * @param rp action在group中的占位情况，默认是大图标
 */
void RibbonGroup::addAction(QAction *act, QToolButton::ToolButtonPopupMode popMode,
                            RibbonGroup::RowProportion rp)
{
    if (act == Q_NULLPTR) {
        return;
    }
    setActionRowProportionProperty(act, rp);
    QWidget::addAction(act);
    RibbonButton *btn = d->lastAddActionButton();

    if (btn) {
        btn->setPopupMode(popMode);
    }
}

/**
 * @brief 生成并添加一个action
 *
 * 如果不对此action做操作，RibbonGroup将管理此action
 *
 * @note action的父对象将设置为RibbonGroup，RibbonGroup在删除时将会删除子对象，会把这个action也删除，
 * 如果不想此action也删除，需要对action重新设置父对象
 *
 * @param text action的文字
 * @param icon action的图标
 * @param popMode 按钮的样式
 * @param rp action在group中的占位情况，默认是大图标
 * @return 返回添加的action
 */
QAction *RibbonGroup::addAction(const QString &text, const QIcon &icon, QToolButton::ToolButtonPopupMode popMode,
                                RibbonGroup::RowProportion rp)
{
    QAction *action = new QAction(icon, text, this);
    addAction(action, popMode, rp);
    return action;
}

/**
 * @brief 添加一个普通菜单
 * @param menu
 * @param rp
 * @param popMode,菜单弹出模式，默认InstantPopup模式
 * @return
 */
RibbonButton *RibbonGroup::addMenu(QMenu *menu, RibbonGroup::RowProportion rp,
                                   QToolButton::ToolButtonPopupMode popMode)
{
    if (menu == Q_NULLPTR) {
        return Q_NULLPTR;
    }
    QAction *action = menu->menuAction();

    addAction(action, rp);
    RibbonButton *btn = d->lastAddActionButton();

    btn->setPopupMode(popMode);
    return btn;
}

RibbonButton *RibbonGroup::addLargeMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode)
{
    return addMenu(menu, RibbonGroup::Large, popMode);
}

RibbonButton *RibbonGroup::addSmallMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode)
{
    return addMenu(menu, RibbonGroup::Small, popMode);
}

/**
 * @brief 添加一个ActionMenu
 * @param action
 * @param menu
 * @param rp
 * @return
 */
RibbonButton *RibbonGroup::addActionMenu(QAction *action, QMenu *menu, RibbonGroup::RowProportion rp)
{
    RibbonButton *btn = addAction(action, rp);
    if (Q_NULLPTR == btn) {
        return Q_NULLPTR;
    }
    btn->setMenu(menu);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    return btn;
}

/**
 * @brief 添加action menu,action menu是一个特殊的menu,即可点击触发action，也可弹出菜单
 * @param action 点击触发的action，在group中，图标以此action的图标为准
 * @param menu 需要弹出的menu
 * @return 返回
 */
RibbonButton *RibbonGroup::addLargeActionMenu(QAction *action, QMenu *menu)
{
    return addActionMenu(action, menu, RibbonGroup::Large);
}

/**
 * @brief 添加窗口
 *
 * @param w
 * @param rp
 * @return 返回action(QWidgetAction)
 * @note RibbonGroup并不会管理此窗口内存，在delete RibbonGroup时，此窗口如果父对象不是RibbonGroup将不会被删除
 */
QAction *RibbonGroup::addWidget(QWidget *w, RibbonGroup::RowProportion rp)
{
    QWidgetAction *action = new QWidgetAction(this);

    action->setDefaultWidget(w);
    w->setAttribute(Qt::WA_Hover);
    setActionRowProportionProperty(action, rp);
    QWidget::addAction(action);
    return action;
}

/**
 * @brief 添加窗口，占用所有行
 * @param w
 * @return 返回action(QWidgetAction)
 */
QAction *RibbonGroup::addLargeWidget(QWidget *w)
{
    return addWidget(w, RibbonGroup::Large);
}

/**
 * @brief 添加窗口,占用ribbon的一行
 * @param w
 * @return
 */
QAction *RibbonGroup::addMediumWidget(QWidget *w)
{
    return addWidget(w, RibbonGroup::Medium);
}

/**
 * @brief 添加窗口,占用ribbon的一行
 * @param w
 * @return 返回action(QWidgetAction)
 */
QAction *RibbonGroup::addSmallWidget(QWidget *w)
{
    return addWidget(w, RibbonGroup::Small);
}

/**
 * @brief RibbonGroup::addGallery
 * @return
 * @note RibbonGroup将拥有RibbonGallery的管理权
 */
RibbonGallery *RibbonGroup::addGallery()
{
    RibbonGallery *gallery = new RibbonGallery(this);

    addWidget(gallery, RibbonGroup::Large);

    setExpanding();
    return gallery;
}

/**
 * @brief 添加分割线
 * @param top 上边距 @default 6
 * @param bottom 下边距 @default 6
 */
QAction *RibbonGroup::addSeparator(int top, int bottom)
{
    QAction *action = new QAction(this);

    action->setSeparator(true);
    setActionRowProportionProperty(action, RibbonGroup::Large);
    QWidget::addAction(action);
    QWidget *w = d->m_layout->lastWidget();
    RibbonSeparator *sep = qobject_cast<RibbonSeparator *>(w);

    if (sep) {
        sep->setTopBottomMargins(top, bottom);
    }
    return action;
}

/**
 * @brief 从group中把action对应的button提取出来，如果action没有对应的button，就返回Q_NULLPTR
 * @param action
 * @return 如果action没有对应的button，就返回Q_NULLPTR
 */
RibbonButton *RibbonGroup::ribbonButtonForAction(QAction *action)
{
    RibbonGroupLayout *lay = qobject_cast<RibbonGroupLayout *>(layout());

    if (lay) {
        int index = lay->indexOf(action);
        if (index == -1) {
            return Q_NULLPTR;
        }
        QLayoutItem *item = lay->takeAt(index);
        RibbonButton *btn = qobject_cast<RibbonButton *>(item ? item->widget() : Q_NULLPTR);
        return btn;
    }
    return Q_NULLPTR;
}

/**
 * @brief 获取group下面的所有toolbutton
 * @return
 */
QList<RibbonButton *> RibbonGroup::ribbonButtons() const
{
    const QObjectList &objs = children();
    QList<RibbonButton *> res;

    for (QObject *o : objs) {
        RibbonButton *b = qobject_cast<RibbonButton *>(o);
        if (b) {
            res.append(b);
        }
    }
    return res;
}

/**
 * @brief 判断是否存在OptionAction
 * @return 存在返回true
 */
bool RibbonGroup::isHaveOptionAction() const
{
    return (d->m_optionActionButton != Q_NULLPTR);
}

/**
 * @brief 添加操作action，如果要去除，传入Q_NULLPTR指针即可，RibbonGroup不会对QAction的所有权进行管理
 * @param action
 * @note 要去除OptionAction直接传入Q_NULLPTR即可
 * @note RibbonGroup不对QAction的destroy进行关联，如果外部对action进行delete，需要先传入Q_NULLPTR给addOptionAction
 */
void RibbonGroup::addOptionAction(QAction *action)
{
    if (Q_NULLPTR == action) {
        if (d->m_optionActionButton) {
            delete d->m_optionActionButton;
            d->m_optionActionButton = Q_NULLPTR;
        }
        return;
    }
    if (Q_NULLPTR == d->m_optionActionButton) {
        d->m_optionActionButton = new RibbonGroupOptionButton(this);
    }
    d->m_optionActionButton->setFixedSize(optionActionButtonSize());
    d->m_optionActionButton->setIconSize(optionActionButtonSize() - QSize(-2, -2));
    d->m_optionActionButton->connectAction(action);
    updateGeometry();   // 通知layout进行重新布局
    repaint();
}

/**
 * @brief action对应的布局index，此操作一般用于移动，其他意义不大
 * @param act
 * @return 没有查到返回-1
 */
int RibbonGroup::actionIndex(QAction *act) const
{
    return d->m_layout->indexOf(act);
}

/**
 * @brief 移动action
 * @param from 要移动action的位置，通过@ref actionIndex 获取
 * @param to 要移动的位置
 */
void RibbonGroup::moveAction(int from, int to)
{
    d->m_layout->move(from, to);
    updateGeometry();   // 通知layout进行重新布局
}

RibbonGroup::GroupLayoutMode RibbonGroup::groupLayoutMode() const
{
    return d->m_groupLayoutMode;
}

/**
 * @brief 设置GroupLayoutMode
 * @param mode
 */
void RibbonGroup::setGroupLayoutMode(RibbonGroup::GroupLayoutMode mode)
{
    if (d->m_groupLayoutMode == mode) {
        return;
    }
    d->m_groupLayoutMode = mode;
    updateItemGeometry();
}

void RibbonGroup::updateItemGeometry()
{
    // reset layout
    layout()->setSpacing(RibbonGroup::TwoRowMode == d->m_groupLayoutMode ? 4 : 2);
    updateGeometry();

    d->resetLargeToolButtonStyle();
}

bool RibbonGroup::isTwoRow() const
{
    return (TwoRowMode == d->m_groupLayoutMode);
}

/**
 * @brief 返回optionActionButton的尺寸
 * @return
 */
QSize RibbonGroup::optionActionButtonSize() const
{
    return (isTwoRow() ? QSize(12, 12) : QSize(16, 16));
}

QSize RibbonGroup::sizeHint() const
{
    QSize laySize = layout()->sizeHint();
    int maxWidth = laySize.width() + 2;

    if (ThreeRowMode == groupLayoutMode()) {
        // 三行模式
        QFontMetrics fm = fontMetrics();
        QSize titleSize = fm.size(Qt::TextShowMnemonic, windowTitle());
        if (d->m_optionActionButton) {
            // optionActionButton的宽度需要预留
            titleSize.setWidth(titleSize.width() + d->m_optionActionButton->width() + 4);
        }
        maxWidth = qMax(maxWidth, titleSize.width());
    }
    return QSize(maxWidth, laySize.height());
}

QSize RibbonGroup::minimumSizeHint() const
{
    return layout()->minimumSize();
}

/**
 * @brief 判断此group是否为（水平）扩展模式
 * @return 是扩展模式返回true
 */
bool RibbonGroup::isExpanding() const
{
    QSizePolicy sp = sizePolicy();

    return (sp.horizontalPolicy() == QSizePolicy::Expanding);
}

/**
 * @brief 把group设置为扩展模式，此时会撑大水平区域
 * @param isExpanding
 */
void RibbonGroup::setExpanding(bool isExpanding)
{
    setSizePolicy(isExpanding ? QSizePolicy::Expanding : QSizePolicy::Preferred, QSizePolicy::Fixed);
}

/**
 * @brief 判断是否可以自定义
 * @return
 */
bool RibbonGroup::isCanCustomize() const
{
    return d->m_isCanCustomize;
}

/**
 * @brief 设置是否可以自定义
 * @param b
 */
void RibbonGroup::setCanCustomize(bool b)
{
    d->m_isCanCustomize = b;
}

/**
 * @brief 获取大图标的高度
 * @return
 */
int RibbonGroup::largeHeight() const
{
    return RibbonGroupLayout::calcLargeHeight(rect(), this);
}

/**
 * @brief 标题栏高度，仅在三行模式下生效
 * @return
 */
int RibbonGroup::titleHeight() const
{
    return (isTwoRow() ? 0 : groupTitleHeight());
}

/**
 * @brief 定义所有的group的标题栏高度，有别于@sa titleHeight 此函数是静态函数，获取的是全局的高度
 * 而 @sa titleHeight 函数会根据当前的行情况返回标题栏高度，在2行情况下返回0
 *
 * @return
 */
int RibbonGroup::groupTitleHeight()
{
    return RibbonGroupPrivate::s_groupTitleHeight;
}

/**
 * @brief 设置group的全局高度，此函数是个全局的影响
 * @note RibbonStyleOption会用到此函数，调用设置函数后需要手动重新计算RibbonStyleOption的内容,@sa
 * RibbonStyleOption::recalc
 * @sa RibbonStyleOption
 * @param h
 */
void RibbonGroup::setGroupTitleHeight(int h)
{
    RibbonGroupPrivate::s_groupTitleHeight = h;
}

void RibbonGroup::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    //! 1. 绘制标题
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
    HELP_DRAW_RECT(p, rect());
#endif
    if (ThreeRowMode == groupLayoutMode()) {
        const int th = titleHeight();
        QFont f = font();
        f.setPixelSize(th * 0.6);
        p.setFont(f);
        if (d->m_optionActionButton) {
            p.drawText(1, height() - th, width() - d->m_optionActionButton->width() - 4, th, Qt::AlignCenter,
                       windowTitle());
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            QRect r = QRect(1, height() - th, width() - d->m_optionActionButton->width() - 4, th - 2);
            HELP_DRAW_RECT(p, r);
#endif
        } else {
            p.drawText(1, height() - th, width(), th, Qt::AlignCenter, windowTitle());
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            QRect r = QRect(1, height() - th, width(), th);
            HELP_DRAW_RECT(p, r);
#endif
        }
    }

    QWidget::paintEvent(event);
}

void RibbonGroup::resizeEvent(QResizeEvent *event)
{
    //! 1.移动操作按钮到角落
    if (d->m_optionActionButton) {
        if (ThreeRowMode == groupLayoutMode()) {
            d->m_optionActionButton->move(width() - d->m_optionActionButton->width() - 2,
                height() - titleHeight() + (titleHeight() - d->m_optionActionButton->height()) / 2);
        } else {
            d->m_optionActionButton->move(width() - d->m_optionActionButton->width(),
                                          height() - d->m_optionActionButton->height());
        }
    }
    //! 2.resize后，重新设置分割线的高度
    //! 由于分割线在布局中，只要分割线足够高就可以，不需要重新设置
    return QWidget::resizeEvent(event);
}

/**
 * @brief 处理action的事件
 *
 * 这里处理了ActionAdded，ActionChanged，ActionRemoved三个事件
 *
 * ActionAdded时向布局请求，添加action，布局中同时触发了@ref RibbonGroupLayout::createItem 函数
 * 此函数用于生成窗口，例如QRibbonButton
 *
 * ActionChanged时会让布局重新计算尺寸，并向page请求重新布局，有可能page的所有要重新调整尺寸
 *
 * ActionRemoved会移除布局管理的QLayoutItem，并进行内存清理，这时窗口也会隐藏，同时销毁
 *
 * @param event
 * @note 所有action事件都会向page请求重新布局
 *
 */
void RibbonGroup::actionEvent(QActionEvent *event)
{
    QAction *action = event->action();
    QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action);

    switch (event->type()) {
    case QEvent::ActionAdded: {
        RibbonGroupLayout *lay = d->m_layout;
        if (Q_NULLPTR != widgetAction) {
            if (widgetAction->parent() != this) {
                widgetAction->setParent(this);
            }
        }
        int index = layout()->count();
        if (event->before()) {
            // 说明是插入
            index = lay->indexOf(action);
            if (-1 == index) {
                index = layout()->count();   // 找不到的时候就插入到最后
            }
        }
        lay->insertAction(index, action, getActionRowProportionProperty(action));
        // 由于group的尺寸发生变化，需要让page也调整
        if (parentWidget()) {
            QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
        }
    } break;

    case QEvent::ActionChanged: {
        // 让布局重新绘制
        layout()->invalidate();
        // 由于group的尺寸发生变化，需要让page也调整
        if (parentWidget()) {
            QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
        }
    } break;

    case QEvent::ActionRemoved: {
        RibbonGroupLayout *lay = d->m_layout;
        action->disconnect(this);
        int index = lay->indexOf(action);
        if (index != -1) {
            QLayoutItem *item = lay->takeAt(index);
            delete item;
        }
        // 由于group的尺寸发生变化，需要让page也调整
        if (parentWidget()) {
            QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
        }
    } break;

    default:
        break;
    }
}
