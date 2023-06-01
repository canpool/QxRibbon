﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonPage.h"
#include "QxRibbonPagePrivate.h"
#include "QxRibbonManager.h"
#include "QxRibbonControls.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLinearGradient>
#include <QList>
#include <QMap>
#include <QPainter>
#include <QResizeEvent>


RibbonPageScrollButton::RibbonPageScrollButton(Qt::ArrowType arr, QWidget *p)
    : QToolButton(p)
{
    setArrowType(arr);
}

RibbonPagePrivate::RibbonPagePrivate(RibbonPage *parent)
    : q(parent)
    , m_ribbonBar(Q_NULLPTR)
    , m_groupLayoutMode(RibbonGroup::ThreeRowMode)
    , m_leftScrollBtn(Q_NULLPTR)
    , m_rightScrollBtn(Q_NULLPTR)
    , m_sizeHint(50, 50)
    , m_contentsMargins(1, 1, 1, 1)
    , m_isRightScrollBtnShow(false)
    , m_isLeftScrollBtnShow(false)
    , m_isPageContext(false)
    , m_isCanCustomize(true)
{
}

RibbonPagePrivate::~RibbonPagePrivate()
{
}

void RibbonPagePrivate::init()
{
    m_leftScrollBtn = new RibbonPageScrollButton(Qt::LeftArrow, q);
    m_rightScrollBtn = new RibbonPageScrollButton(Qt::RightArrow, q);
    m_leftScrollBtn->setVisible(false);
    m_rightScrollBtn->setVisible(false);
    connect(m_leftScrollBtn, &QToolButton::clicked, this, &RibbonPagePrivate::onLeftScrollButtonClicked);
    connect(m_rightScrollBtn, &QToolButton::clicked, this, &RibbonPagePrivate::onRightScrollButtonClicked);
}

RibbonGroup *RibbonPagePrivate::addGroup(const QString &title)
{
    return insertGroup(m_itemList.size(), title);
}

RibbonGroup *RibbonPagePrivate::insertGroup(int index, const QString &title)
{
    RibbonGroup *group = new RibbonGroup(q);

    group->setWindowTitle(title);
    group->setObjectName(title);
    insertGroup(index, group);
    return group;
}

void RibbonPagePrivate::addGroup(RibbonGroup *group)
{
    insertGroup(m_itemList.size(), group);
}

void RibbonPagePrivate::insertGroup(int index, RibbonGroup *group)
{
    if (Q_NULLPTR == group) {
        return;
    }
    if (group->parentWidget() != q) {
        group->setParent(q);
    }
    group->setGroupLayoutMode(m_groupLayoutMode);
    group->installEventFilter(q);
    index = qMax(0, index);
    index = qMin(m_itemList.size(), index);
    RibbonPageItem item;

    item.m_group = group;
    // 分割线
    item.m_separator = new RibbonSeparator(q);
    // 插入list中
    m_itemList.insert(index, item);
    // 标记需要重新计算尺寸
    updateItemGeometry();
    group->setVisible(true);
}

bool RibbonPagePrivate::takeGroup(RibbonGroup *group)
{
    RibbonPageItem item;

    for (int i = 0; i < m_itemList.size(); ++i) {
        if (group == m_itemList[i].m_group) {
            item = m_itemList.takeAt(i);
        }
    }
    if (item.isNull()) {
        return false;
    }
    if (item.m_separator) {
        item.m_separator->hide();
        // 对应的分割线删除，但group不删除
        item.m_separator->deleteLater();
    }
    return true;
}

bool RibbonPagePrivate::removeGroup(RibbonGroup *group)
{
    if (takeGroup(group)) {
        group->hide();
        group->deleteLater();
        return true;
    }
    return false;
}

QList<RibbonGroup *> RibbonPagePrivate::groupList()
{
    QList<RibbonGroup *> res;

    for (const RibbonPageItem &i : m_itemList) {
        if (!i.isNull()) {
            res.append(i.m_group);
        }
    }
    return res;
}

/**
 * @brief 设置group的模式
 *
 * 在@ref RibbonBar调用@ref RibbonBar::setRibbonStyle 函数时，会对所有的RibbonPage调用此函数
 * 把新的RibbonGroup::GroupLayoutMode设置进去
 *
 * 此函数为RibbonPage::setGroupLayoutMode的代理，
 * 在RibbonPage类中，此函数为protected，主要在RibbonBar::setRibbonStyle时才会触发
 * @param m
 */
void RibbonPagePrivate::setGroupLayoutMode(RibbonGroup::GroupLayoutMode m)
{
    if (m_groupLayoutMode == m) {
        return;
    }
    m_groupLayoutMode = m;
    QList<RibbonGroup *> groups = groupList();

    for (RibbonGroup *group : groups) {
        group->setGroupLayoutMode(m);
    }
    updateItemGeometry();
}

void RibbonPagePrivate::setBackgroundBrush(const QBrush &brush)
{
    QPalette p = q->palette();

    p.setBrush(QPalette::Window, brush);
    q->setPalette(p);
}

/**
 * @brief 计算所有元素的SizeHint宽度总和
 * @return
 */
int RibbonPagePrivate::totalSizeHintWidth() const
{
    int total = 0;
    QMargins mag = m_contentsMargins;

    if (!mag.isNull()) {
        total += (mag.left() + mag.right());
    }
    // 先计算总长
    for (RibbonPageItem item : m_itemList) {
        if (item.isEmpty()) {
            // 如果是hide就直接跳过
            continue;
        }
        QSize groupSize = item.m_group->sizeHint();
        QSize separatorSize(0, 0);
        if (item.m_separator) {
            separatorSize = item.m_separator->sizeHint();
        }
        total += groupSize.width();
        total += separatorSize.width();
    }
    return total;
}

QSize RibbonPagePrivate::pageContentSize() const
{
    const RibbonPage *page = q;
    QSize s = page->size();
    QMargins mag = m_contentsMargins;

    if (!mag.isNull()) {
        s.rheight() -= (mag.top() + mag.bottom());
        s.rwidth() -= (mag.right() + mag.left());
    }
    return s;
}

void RibbonPagePrivate::updateItemGeometry()
{
    RibbonPage *page = q;
    QSize contentSize = pageContentSize();
    int y = 0;

    if (!m_contentsMargins.isNull()) {
        y = m_contentsMargins.top();
    }
    // total 是总宽，不是x坐标系，x才是坐标系
    int total = totalSizeHintWidth();
    // 记录可以扩展的数量
    int canExpandingCount = 0;
    // 扩展的宽度
    int expandWidth = 0;

    // 如果total < pageWidth,d->mXBase可以设置为0
    // 判断是否超过总长度
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "\r\n\r\n============================================="
             << "\r\nRibbonPagePrivate::updateItemGeometry"
             << "\r\ngroup name:" << page->windowTitle() << "\r\n height:" << height
             << "\r\n first total:" << total << "\r\n y:" << y << "\r\n expandWidth:" << expandWidth;
#endif
    if (total <= contentSize.width()) {
        // 这个是避免一开始totalWidth > pageSize.width()，通过滚动按钮调整了d->mBaseX
        // 随之调整了窗体尺寸，调整后totalWidth < pageSize.width()导致page在原来位置
        // 无法显示，必须这里把mBaseX设置为0
        m_XBase = 0;
        //
        for (const RibbonPageItem &item : m_itemList) {
            if (!item.isEmpty()) {
                if (item.m_group->isExpanding()) {
                    // group可扩展
                    ++canExpandingCount;
                }
            }
        }
        // 计算可扩展的宽度
        if (canExpandingCount > 0) {
            expandWidth = (contentSize.width() - total) / canExpandingCount;
        } else {
            expandWidth = 0;
        }
    }
    total = 0;   // total重新计算
    int x = m_XBase;

    // 先按照sizeHint设置所有的尺寸
    for (RibbonPageItem &item : m_itemList) {
        if (item.isEmpty()) {
            // 如果是hide就直接跳过
            if (item.m_separator) {
                // group hide分割线也要hide
                item.m_separator->hide();
            }
            item.m_willSetGroupGeometry = QRect(0, 0, 0, 0);
            item.m_willSetSeparatorGeometry = QRect(0, 0, 0, 0);
            continue;
        }
        RibbonGroup *group = item.m_group;
        if (Q_NULLPTR == group) {
            qDebug() << "unknow widget in RibbonPageLayout";
            continue;
        }
        QSize groupSize = group->sizeHint();
        QSize separatorSize(0, 0);
        if (item.m_separator) {
            separatorSize = item.m_separator->sizeHint();
        }
        if (group->isExpanding()) {
            // 可扩展，就把group扩展到最大
            groupSize.setWidth(groupSize.width() + expandWidth);
        }
        int w = groupSize.width();
        item.m_willSetGroupGeometry = QRect(x, y, w, contentSize.height());
        x += w;
        total += w;
        w = separatorSize.width();
        item.m_willSetSeparatorGeometry = QRect(x, y, w, contentSize.height());
        x += w;
        total += w;
    }
    m_totalWidth = total;
    // 判断滚动按钮是否显示
    if (total > contentSize.width()) {
        // 超过总长度，需要显示滚动按钮
        if (0 == m_XBase) {
            // 已经移动到最左，需要可以向右移动
            m_isRightScrollBtnShow = true;
            m_isLeftScrollBtnShow = false;
        } else if (m_XBase <= (contentSize.width() - total)) {
            // 已经移动到最右，需要可以向左移动
            m_isRightScrollBtnShow = false;
            m_isLeftScrollBtnShow = true;
        } else {
            // 移动到中间两边都可以动
            m_isRightScrollBtnShow = true;
            m_isLeftScrollBtnShow = true;
        }
    } else {
        // 说明total 小于 pageWidth
        m_isRightScrollBtnShow = false;
        m_isLeftScrollBtnShow = false;
    }
    QWidget *cp = page->parentWidget();
    int parentHeight = (Q_NULLPTR == cp) ? contentSize.height() : cp->height();
    int parentWidth = (Q_NULLPTR == cp) ? total : cp->width();
    m_sizeHint = QSize(parentWidth, parentHeight);
    doItemLayout();
}

void RibbonPagePrivate::doItemLayout()
{
    RibbonPage *page = q;

    // 两个滚动按钮的位置永远不变
    m_leftScrollBtn->setGeometry(0, 0, 12, page->height());
    m_rightScrollBtn->setGeometry(page->width() - 12, 0, 12, page->height());
    QList<QWidget *> showWidgets, hideWidgets;

    for (const RibbonPageItem &item : m_itemList) {
        if (item.isNull()) {
            continue;
        }
        if (item.isEmpty()) {
            hideWidgets << item.m_group;
            if (item.m_separator) {
                hideWidgets << item.m_separator;
            }
        } else {
            // item.m_group->setFixedSize(item.m_willSetGroupGeometry.size());
            // item.m_group->move(item.m_willSetGroupGeometry.topLeft());
            item.m_group->setGeometry(item.m_willSetGroupGeometry);
            showWidgets << item.m_group;
            if (item.m_separator) {
                item.m_separator->setGeometry(item.m_willSetSeparatorGeometry);
                showWidgets << item.m_separator;
            }
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            qDebug() << "RibbonPageLayout::doLayout() =";
            qDebug() << "\r\n     group:" << item.m_group->windowTitle()
                     << "\r\n     group geo:" << item.m_willSetGroupGeometry
                     << "\r\n     sep geo:" << item.m_willSetSeparatorGeometry;
#endif
        }
    }

    m_rightScrollBtn->setVisible(m_isRightScrollBtnShow);
    m_leftScrollBtn->setVisible(m_isLeftScrollBtnShow);
    if (m_isRightScrollBtnShow) {
        m_rightScrollBtn->raise();
    }
    if (m_isLeftScrollBtnShow) {
        m_leftScrollBtn->raise();
    }
    // 不在上面那里进行show和hide因为这会触发RibbonGroupLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget *w : showWidgets) {
        w->show();
    }
    for (QWidget *w : hideWidgets) {
        w->hide();
    }
}

void RibbonPagePrivate::doWheelEvent(QWheelEvent *event)
{
    QSize contentSize = pageContentSize();
    // 求总宽
    int totalWidth = m_totalWidth;

    if (totalWidth > contentSize.width()) {
        // 这个时候滚动有效
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;
        int scrollpix = 0;
        if (!numPixels.isNull())
            scrollpix = numPixels.x() / 4;
        else if (!numDegrees.isNull())
            scrollpix = numDegrees.x() / 15;
        else {
        }
        if (scrollpix > 0) {   // 当滚轮向上滑，RibbonPage向左走
            int tmp = m_XBase - scrollpix;
            if (tmp < (contentSize.width() - totalWidth)) {
                tmp = contentSize.width() - totalWidth;
            }
            m_XBase = tmp;
        } else {                            // 当滚轮向下滑，RibbonPage向右走
            int tmp = m_XBase - scrollpix;   // 此时numDegrees为负数
            if (tmp > 0) {
                tmp = 0;
            }
            m_XBase = tmp;
        }
        updateItemGeometry();
    } else {
        // 这时候无需处理事件，把滚动事件上发让父级也能接收
        event->ignore();
        m_XBase = 0;
    }
}

void RibbonPagePrivate::onLeftScrollButtonClicked()
{
    int width = pageContentSize().width();
    // 求总宽
    int totalWidth = m_totalWidth;

    if (totalWidth > width) {
        int tmp = m_XBase + width;
        if (tmp > 0) {
            tmp = 0;
        }
        m_XBase = tmp;
    } else {
        m_XBase = 0;
    }
    updateItemGeometry();
}

void RibbonPagePrivate::onRightScrollButtonClicked()
{
    int width = pageContentSize().width();
    // 求总宽
    int totalWidth = m_totalWidth;

    if (totalWidth > width) {
        int tmp = m_XBase - width;
        if (tmp < (width - totalWidth)) {
            tmp = width - totalWidth;
        }
        m_XBase = tmp;
    } else {
        m_XBase = 0;
    }
    updateItemGeometry();
}

RibbonPage::RibbonPage(QWidget *p)
    : QWidget(p)
    , d(new RibbonPagePrivate(this))
{
    d->init();
    setAutoFillBackground(true);
    setBackgroundBrush(Qt::white);
}

RibbonPage::~RibbonPage()
{
    delete d;
}

/**
 * @brief page的名字,等同windowTitle函数
 * @return
 */
QString RibbonPage::pageName() const
{
    return windowTitle();
}

/**
 * @brief 设置 page 名字，等同 setWindowTitle
 * @param title
 */
void RibbonPage::setPageName(const QString &title)
{
    setWindowTitle(title);
}

RibbonGroup::GroupLayoutMode RibbonPage::groupLayoutMode() const
{
    return d->m_groupLayoutMode;
}

/**
 * @brief 设置group的模式
 *
 * 在@ref RibbonBar 调用@ref RibbonBar::setRibbonStyle 函数时，会对所有的RibbonPage调用此函数
 * 把新的RibbonGroup::GroupLayoutMode设置进去
 * @param m
 */
void RibbonPage::setGroupLayoutMode(RibbonGroup::GroupLayoutMode m)
{
    d->setGroupLayoutMode(m);
}

/**
 * @brief 添加group
 *
 * @note group的所有权由RibbonPage来管理，请不要在外部对其进行销毁
 * @param title group的标题，在office/wps的三行模式下会显示在group的下方
 * @return 返回生成的@ref RibbonGroup 指针
 * @see 对RibbonGroup的其他操作，参考 @ref RibbonPage::takeGroup
 */
RibbonGroup *RibbonPage::addGroup(const QString &title)
{
    return d->addGroup(title);
}

/**
 * @brief 添加group
 * @param group group的所有权RibbonPage来管理
 */
void RibbonPage::addGroup(RibbonGroup *group)
{
    d->addGroup(group);
}

/**
 * @brief 新建一个group，并插入到index位置
 * @param title group的title
 * @param index 插入的位置，如果index超出page里group的个数，将插入到最后
 * @return 返回生成的@ref RibbonGroup 指针
 * @note 如果
 */
RibbonGroup *RibbonPage::insertGroup(int index, const QString &title)
{
    return d->insertGroup(index, title);
}

/**
 * @brief 通过索引找到group，如果超过索引范围，会返回Q_NULLPTR
 * @param index
 * @return 如果超过索引范围，会返回Q_NULLPTR
 */
RibbonGroup *RibbonPage::group(int index) const
{
    return d->m_itemList.value(index).m_group;
}

/**
 * @brief 通过名字查找group
 * @param title
 * @return 如果有重名，只会返回第一个符合条件的
 */
RibbonGroup *RibbonPage::groupByName(const QString &title) const
{
    for (RibbonPageItem &i : d->m_itemList) {
        if (i.m_group) {
            if (i.m_group->windowTitle() == title) {
                return i.m_group;
            }
        }
    }
    return Q_NULLPTR;
}

/**
 * @brief 通过ObjectName查找group
 * @param objname
 * @return
 */
RibbonGroup *RibbonPage::groupByObjectName(const QString &objname) const
{
    for (RibbonPageItem &i : d->m_itemList) {
        if (i.m_group) {
            if (i.m_group->objectName() == objname) {
                return i.m_group;
            }
        }
    }
    return Q_NULLPTR;
}

/**
 * @brief 查找group对应的索引
 * @param group
 * @return 如果找不到，返回-1
 */
int RibbonPage::groupIndex(RibbonGroup *group) const
{
    int c = groupCount();

    for (int i = 0; i < c; ++i) {
        if (d->m_itemList[i].m_group == group) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 移动一个Group从from index到to index
 * @param from 要移动group的index
 * @param to 要移动到的位置
 */
void RibbonPage::moveGroup(int from, int to)
{
    if (from == to) {
        return;
    }
    if (to < 0) {
        to = 0;
    }
    if (to >= groupCount()) {
        to = groupCount() - 1;
    }
    d->m_itemList.move(from, to);
    d->updateItemGeometry();
}

/**
 * @brief 把group脱离RibbonPage的管理
 * @param group 需要提取的group
 * @return 成功返回true，否则返回false
 */
bool RibbonPage::takeGroup(RibbonGroup *group)
{
    return d->takeGroup(group);
}

/**
 * @brief 移除Group，Page会直接回收RibbonGroup内存
 * @param group 需要移除的group
 * @note 移除后group为野指针，一般操作完建议把group指针设置为Q_NULLPTR
 *
 * 此操作等同于：
 *
 * @code
 * RibbonGroup* group;
 * ...
 * page->takeGroup(group);
 * group->hide();
 * group->deleteLater();
 * @endcode
 */
bool RibbonPage::removeGroup(RibbonGroup *group)
{
    return d->removeGroup(group);
}

/**
 * @brief 移除group
 * @param index group的索引，如果超出会返回false
 * @return 成功返回true
 */
bool RibbonPage::removeGroup(int index)
{
    RibbonGroup *grp = group(index);

    if (Q_NULLPTR == grp) {
        return false;
    }
    return removeGroup(grp);
}

/**
 * @brief 返回Page下的所有group
 * @return
 */
QList<RibbonGroup *> RibbonPage::groupList() const
{
    return d->groupList();
}

///
/// \brief RibbonPage::setBackgroundBrush
/// \param brush
///
void RibbonPage::setBackgroundBrush(const QBrush &brush)
{
    d->setBackgroundBrush(brush);
}

QSize RibbonPage::sizeHint() const
{
    return d->m_sizeHint;
}

/**
 * @brief 如果是ContextPage，此函数返回true
 * @return
 */
bool RibbonPage::isPageContext() const
{
    return d->m_isPageContext;
}

/**
 * @brief 返回group的个数
 * @return
 */
int RibbonPage::groupCount() const
{
    return d->m_itemList.size();
}

/**
 * @brief 判断是否可以自定义
 * @return
 */
bool RibbonPage::isCanCustomize() const
{
    return d->m_isCanCustomize;
}

/**
 * @brief 设置是否可以自定义
 * @param b
 */
void RibbonPage::setCanCustomize(bool b)
{
    d->m_isCanCustomize = b;
}

/**
 * @brief 获取对应的ribbonbar
 * @return 如果没有加入ribbonbar的管理，此值为null
 */
RibbonBar *RibbonPage::ribbonBar() const
{
    return d->m_ribbonBar;
}

/**
 * @brief 刷新page的布局，适用于改变ribbon的模式之后调用
 */
void RibbonPage::updateItemGeometry()
{
    QList<RibbonGroup *> groups = groupList();
    for (RibbonGroup *group : groups) {
        group->updateItemGeometry();
    }
    d->updateItemGeometry();
}

/**
 * @brief 标记这个是上下文标签
 * @param isPageContext
 */
void RibbonPage::markIsPageContext(bool isPageContext)
{
    d->m_isPageContext = isPageContext;
}

/**
 * @brief RibbonPage::event
 * @param event
 * @return
 */
bool RibbonPage::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LayoutRequest: {
        d->updateItemGeometry();
    } break;

    default:
        break;
    }
    return QWidget::event(e);
}

void RibbonPage::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    // 尺寸没变就不用管
    // QSize delta = e->size() - e->oldSize();
    // if (delta.isNull()) {
    //     return;
    // }
    d->updateItemGeometry();
}

bool RibbonPage::eventFilter(QObject *watched, QEvent *event)
{
    if (Q_NULLPTR == watched) {
        return false;
    }
    RibbonGroup *group = qobject_cast<RibbonGroup *>(watched);

    if (group) {
        switch (event->type()) {
        case QEvent::HideToParent: {
            // 隐藏和显示都要重新布局
            //  layout()->invalidate();
        } break;

        case QEvent::ShowToParent: {
            // 隐藏和显示都要重新布局
            //  layout()->invalidate();
        } break;

        default:
            break;
        }
    }
    return false;
}

/**
 * @brief 在超出边界情况下，滚轮可滚动group
 * @param event
 */
void RibbonPage::wheelEvent(QWheelEvent *event)
{
    d->doWheelEvent(event);
}

/**
 * @brief 设置ribbonbar，此函数仅提供给ribbonbar调用
 * @param bar ribbonbar指针
 */
void RibbonPage::setRibbonBar(RibbonBar *bar)
{
    d->m_ribbonBar = bar;
}
