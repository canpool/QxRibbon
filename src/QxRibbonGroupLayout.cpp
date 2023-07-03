/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonGroupLayout.h"
#include "QxRibbonPage.h"
#include "QxRibbonManager.h"
#include "QxRibbonGallery.h"
#include "QxRibbonGroup.h"
#include "QxRibbonControls.h"

#include <QQueue>
#include <QWidgetAction>
#include <set>

#define RibbonGroupLayout_DEBUG_PRINT 0
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

RibbonGroupItem::RibbonGroupItem(QWidget *widget)
    : QWidgetItem(widget)
    , action(Q_NULLPTR)
    , customWidget(false)
{
}

bool RibbonGroupItem::isEmpty() const
{
    return (action == Q_NULLPTR || !action->isVisible());
}

class RibbonGroupLayoutPrivate
{
public:
    RibbonGroupLayoutPrivate();

    // 返回所有列的区域
    //  QMap<int,QRect> columnsGeometry() const;
    // 根据列数，计算窗口的宽度，以及最大宽度
    void columnWidthInfo(int colindex, int &width, int &maximum) const;

    // 重新计算扩展条码，此函数必须在updateGeomArray函数之后调用
    void recalcExpandGeomArray(const QRect &setrect);

    RibbonGroupItem *createItem(QAction *action, RibbonGroup::RowProportion rp = RibbonGroup::Auto);
    void updateGeomArray(const QRect &setrect);
    void layoutActions();

public:
    RibbonGroupLayout *q;
    QList<RibbonGroupItem *> m_items;
    QSize m_sizeHint;    ///< sizeHint返回的尺寸
    static QMargins s_contentsMargins;
    int m_columnCount;   ///< 记录有多少列
    int m_largeHeight;   ///< 记录大图标的高度
    bool m_expandFlag;   ///< 标记是否是会扩展的
    bool m_dirty;        ///< 用于标记是否需要刷新元素，参考 QToolBarLayout 源码
};

QMargins RibbonGroupLayoutPrivate::s_contentsMargins = QMargins(1, 1, 1, 1);

RibbonGroupLayoutPrivate::RibbonGroupLayoutPrivate()
    : m_columnCount(0)
    , m_expandFlag(false)
    , m_dirty(true)
{

}

/**
 * @brief 根据列数，计算窗口的宽度，以及最大宽度
 * @param colindex
 * @param width 如果传入没有这个列，返回-1
 * @param maximum 如果传入没有这个列，返回-1
 */
void RibbonGroupLayoutPrivate::columnWidthInfo(int colindex, int &width, int &maximum) const
{
    width = -1;
    maximum = -1;
    for (RibbonGroupItem *item : m_items) {
        if (!item->isEmpty() && (item->columnIndex == colindex)) {
            width = qMax(width, item->itemWillSetGeometry.width());
            maximum = qMax(maximum, item->widget()->maximumWidth());
        }
    }
}

void RibbonGroupLayoutPrivate::recalcExpandGeomArray(const QRect &setrect)
{
    // 计算能扩展的尺寸
    int expandwidth = setrect.width() - m_sizeHint.width();

    if (expandwidth <= 0) {
        // 没有必要设置
        return;
    }
    // 列扩展信息
    struct _columnExpandInfo {
        int oldColumnWidth = 0;        ///< 原来的列宽
        int columnMaximumWidth = -1;   ///< 列的最大宽度
        int columnExpandedWidth = 0;   ///< 扩展后列的宽度
        QList<RibbonGroupItem *> expandItems;
    };
    // 此变量用于记录可以水平扩展的列和控件，在布局结束后，如果还有空间，就把水平扩展的控件进行扩展
    QMap<int, _columnExpandInfo> columnExpandInfo;

    for (RibbonGroupItem *item : qAsConst(m_items)) {
        if ((!item->isEmpty()) && item->expandingDirections() & Qt::Horizontal) {
            // 只获取可见的
            QMap<int, _columnExpandInfo>::iterator i = columnExpandInfo.find(item->columnIndex);
            if (i == columnExpandInfo.end()) {
                i = columnExpandInfo.insert(item->columnIndex, _columnExpandInfo());
            }
            i.value().expandItems.append(item);
        }
    }
    if (columnExpandInfo.size() <= 0) {
        // 没有需要扩展的就退出
        return;
    }
    // 获取完可扩展的列和控件后，计算对应的列的尺寸
    // 计算能扩展的尺寸
    int oneColCanexpandWidth = expandwidth / columnExpandInfo.size();

    for (QMap<int, _columnExpandInfo>::iterator i = columnExpandInfo.begin(); i != columnExpandInfo.end();) {
        int &oldColumnWidth = i.value().oldColumnWidth;
        int &columnMaximumWidth = i.value().columnMaximumWidth;
        columnWidthInfo(i.key(), oldColumnWidth, columnMaximumWidth);
        if ((oldColumnWidth <= 0) || (oldColumnWidth > columnMaximumWidth)) {
            // 如果小于0说明没有这个列，这种属于异常，删除继续
            //  oldColumnWidth > columnMaximumWidth也是异常
            i = columnExpandInfo.erase(i);
            continue;
        }
        // 开始调整
        int colwidth = oneColCanexpandWidth + oldColumnWidth;   // 先扩展了
        if (colwidth >= columnMaximumWidth) {
            // 过最大宽度要求
            i.value().columnExpandedWidth = columnMaximumWidth;
        } else {
            i.value().columnExpandedWidth = colwidth;
        }
        ++i;
    }
    // 从新调整尺寸
    // 由于会涉及其他列的变更，因此需要所有都遍历一下
    for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
        int moveXLen = i.value().columnExpandedWidth - i.value().oldColumnWidth;
        for (RibbonGroupItem *item : this->m_items) {
            if (item->isEmpty() || (item->columnIndex < i.key())) {
                // 之前的列不用管
                continue;
            }
            if (item->columnIndex == i.key()) {
                // 此列的扩展
                if (i.value().expandItems.contains(item)) {
                    // 此列需要扩展的item才扩展尺寸
                    item->itemWillSetGeometry.setWidth(i.value().columnExpandedWidth);
                } else {
                    // 此列不扩展的模块保持原来的尺寸
                    continue;
                }
            } else {
                // 后面的移动
                item->itemWillSetGeometry.moveLeft(item->itemWillSetGeometry.x() + moveXLen);
            }
        }
    }
}

/**
 * @brief 把action转换为item
 *
 * 此函数参考QToolBarItem *QToolBarLayout::createItem(QAction *action)
 *
 * 对于普通QAction，此函数会创建RibbonButton，RibbonButton的类型参考RibbonGroupItem::RowProportion，
 * @param action
 * @param rp 行高占比情况
 * @return 转换的RibbonGroupItem
 * @note 每个RibbonGroupItem最终都会携带一个widget，传入的是QWidgetAction的话，会直接使用QWidgetAction带的widget，
 * 否则会内部生成一个RibbonButton
 *
 */
RibbonGroupItem *RibbonGroupLayoutPrivate::createItem(QAction *action, RibbonGroup::RowProportion rp)
{
    bool customWidget = false;
    QWidget *widget = Q_NULLPTR;
    RibbonGroup *group = qobject_cast<RibbonGroup *>(q->parentWidget());

    if (!group) {
        return (Q_NULLPTR);
    }
    if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action)) {
        widget = widgetAction->requestWidget(group);
        if (widget != Q_NULLPTR) {
            widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            customWidget = true;   // 标记为true，在移除的时候是不会对这个窗口进行删除，false默认会进行删除如RibbonSeparator和RibbonButton
        }
    } else if (action->isSeparator()) {
        RibbonSeparator *sep = new RibbonSeparator(group);
        widget = sep;
    }
    // 不是widget，自动生成 RibbonButton
    if (!widget) {
        RibbonButton::RibbonButtonType buttonType =
            ((rp == RibbonGroup::Large) ? RibbonButton::LargeButton : RibbonButton::SmallButton);

        RibbonButton *button = new RibbonButton(group);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setButtonType(buttonType);
        if (RibbonButton::LargeButton == buttonType) {
            // 根据group的模式设置button样式
            button->setLargeButtonType((group->isTwoRow()) ? RibbonButton::Lite : RibbonButton::Normal);
        }
        button->setDefaultAction(action);
        // 根据QAction的属性设置按钮的大小

        QObject::connect(button, &RibbonButton::triggered, group, &RibbonGroup::actionTriggered);
        widget = button;
    }
    // 这时总会有widget
    widget->hide();
    RibbonGroupItem *result = new RibbonGroupItem(widget);

    result->rowProportion = rp;
    result->customWidget = customWidget;
    result->action = action;
    return result;
}

/**
 * @brief 更新尺寸
 */
void RibbonGroupLayoutPrivate::updateGeomArray(const QRect &setrect)
{
    RibbonGroup *group = qobject_cast<RibbonGroup *>(q->parentWidget());

    if (!group) {
        return;
    }

#if RibbonGroupLayout_DEBUG_PRINT
    qDebug() << "RibbonGroupLayout::updateGeomArray(" << setrect << ")";
#endif
    int height = setrect.height();
    const QMargins &mag = q->groupContentsMargins();
    const int spacing = q->spacing();
    int x = mag.left();
    // 获取group的布局模式 3行或者2行
    //  rowcount 是ribbon的行，有2行和3行两种
    const short rowCount = (group->groupLayoutMode() == RibbonGroup::ThreeRowMode) ? 3 : 2;
    // largeHeight是对应large占比的高度,group->titleHeight()在两行模式返回0
    const int largeHeight = q->calcLargeHeight(setrect, group);

    m_largeHeight = largeHeight;
    // 计算smallHeight的高度
    const int smallHeight = (largeHeight - (rowCount - 1) * spacing) / rowCount;
    // Medium行的y位置
    const int yMediumRow0 = (2 == rowCount) ? mag.top() : (mag.top() + ((largeHeight - 2 * smallHeight) / 3));
    const int yMediumRow1 = (2 == rowCount) ? (mag.top() + smallHeight + spacing)
                                            : (mag.top() + ((largeHeight - 2 * smallHeight) / 3) * 2 + smallHeight);
    // Small行的y位置
    const int ySmallRow0 = mag.top();
    const int ySmallRow1 = mag.top() + smallHeight + spacing;
    const int ySmallRow2 = mag.top() + 2 * (smallHeight + spacing);
    // row用于记录下个item应该属于第几行，item->rowIndex用于记录当前处于第几行，
    // item->rowIndex主要用于RibbonGroupItem::Medium
    short row = 0;
    int column = 0;
    // 记录每列最大的宽度
    int columnMaxWidth = 0;
    // 记录总宽度
    int totalWidth = 0;

    int itemCount = m_items.count();

#if RibbonGroupLayout_DEBUG_PRINT
    qDebug() << "\r\n\r\n============================================="
             << "\r\nRibbonGroupLayout::updateGeomArray()"
             << " setrect:" << setrect << "\r\ngroup name:" << group->windowTitle()
             << "\r\n largeHeight:" << largeHeight << "\r\n smallHeight:" << smallHeight
             << "\r\n rowCount:" << rowCount;
#endif

    // 本列第一、二行占比
    RibbonGroup::RowProportion thisColumnRP0 = RibbonGroup::Auto;
    RibbonGroupItem *lastGeomItem = Q_NULLPTR;   // 记录最后一个设置位置的item
    for (int i = 0; i < itemCount; ++i) {
        RibbonGroupItem *item = m_items.at(i);
        if (item->isEmpty()) {
            // 如果是hide就直接跳过
#if RibbonGroupLayout_DEBUG_PRINT
            qDebug() << item->widget()->metaObject()->className() << "is hide"
                     << " row:" << row << " col:" << column;
#endif
            item->rowIndex = -1;
            item->columnIndex = -1;
            continue;
        }

        QSize hint = item->sizeHint();
        Qt::Orientations exp = item->expandingDirections();
        if (item->widget()) {
            // 有窗口是水平扩展，则标记为扩展
            if ((item->widget()->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag)) {
                m_expandFlag = true;
            }
        }
        RibbonGroup::RowProportion rp = item->rowProportion;
        if (RibbonGroup::Auto == rp) {
            // 为定义行占比但是垂直扩展，就定义为Large占比，否则就是small占比
            if (exp & Qt::Vertical) {
                rp = RibbonGroup::Large;
            } else {
                rp = RibbonGroup::Small;
            }
        }
        // 开始根据占比和layoutmode来布局
        switch (rp) {
        case RibbonGroup::Large: {
            // ！！在Large，如果不是处于新列的第一行，就需要进行换列处理
            // 把large一直设置在下一列的开始
            if (row != 0) {
                x += (columnMaxWidth + spacing);
                ++column;
                row = 0;
                columnMaxWidth = 0;
            }
            //
            item->rowIndex = 0;
            item->columnIndex = column;
            item->itemWillSetGeometry = QRect(x, mag.top(), hint.width(), largeHeight);
            columnMaxWidth = hint.width();
            // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
            x += (columnMaxWidth + spacing);
            row = 0;
            columnMaxWidth = 0;
            ++column;
        } break;

        case RibbonGroup::Medium: {
            // 2行模式下Medium和small等价
            if (2 == rowCount) {
                if (0 == row) {
                    item->rowIndex = 0;
                    item->columnIndex = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0 = RibbonGroup::Medium;
                    columnMaxWidth = hint.width();
                    // 下个row为1
                    row = 1;
                    // x不变
                } else {
                    item->rowIndex = 1;
                    item->columnIndex = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    // 和上个进行比较得到最长宽度
                    columnMaxWidth = qMax(columnMaxWidth, hint.width());
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columnMaxWidth + spacing);
                    row = 0;
                    columnMaxWidth = 0;
                    ++column;
                }
            } else {
                // 3行模式
                if (0 == row) {
                    item->rowIndex = 0;
                    item->columnIndex = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0 = RibbonGroup::Medium;
                    columnMaxWidth = hint.width();
                    row = 1;
                    // x不变
                } else if (1 == row) {
                    item->rowIndex = 1;
                    item->columnIndex = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    columnMaxWidth = qMax(columnMaxWidth, hint.width());
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columnMaxWidth + spacing);
                    row = 0;
                    columnMaxWidth = 0;
                    ++column;
                } else {
                    // 这种模式一般情况会发生在当前列前两行是Small，添加了一个Medium
                    // 这时需要先换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columnMaxWidth + spacing);
                    ++column;
                    row = 0;
                    columnMaxWidth = 0;
                    // 换列后此时等价于0 == row
                    item->rowIndex = 0;
                    item->columnIndex = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0 = RibbonGroup::Medium;
                    columnMaxWidth = hint.width();
                    row = 1;
                }
            }
        } break;

        case RibbonGroup::Small: {
            if (0 == row) {
                // 第一行
                item->rowIndex = 0;
                item->columnIndex = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow0, hint.width(), smallHeight);
                thisColumnRP0 = RibbonGroup::Small;
                columnMaxWidth = hint.width();
                // 下个row为1
                row = 1;
                // x不变
            } else if (1 == row) {
                // 第二行
                item->rowIndex = 1;
                item->columnIndex = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow1, hint.width(), smallHeight);
                if ((3 == rowCount) && (RibbonGroup::Medium == thisColumnRP0)) {
                    // 三行模式，并且第一行是Medium
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                }
                // 和上个进行比较得到最长宽度
                columnMaxWidth = qMax(columnMaxWidth, hint.width());
                // 这里要看两行还是三行，确定是否要换列
                if (2 == rowCount) {
                    // 两行模式，换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columnMaxWidth + spacing);
                    row = 0;
                    columnMaxWidth = 0;
                    ++column;
                } else {
                    // 三行模式，继续增加行数
                    row = 2;
                    // x不变
                }
                if ((3 == rowCount) && (RibbonGroup::Medium == thisColumnRP0)) {
                    // 三行模式，并且第一行是Medium，换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columnMaxWidth + spacing);
                    row = 0;
                    columnMaxWidth = 0;
                    ++column;
                }
            } else {
                // 第三行
                item->rowIndex = 2;
                item->columnIndex = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow2, hint.width(), smallHeight);
                // 和上个进行比较得到最长宽度
                columnMaxWidth = qMax(columnMaxWidth, hint.width());
                // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                x += (columnMaxWidth + spacing);
                row = 0;
                columnMaxWidth = 0;
                ++column;
            }
        } break;

        default:
            // 不可能出现
            break;
        }
        lastGeomItem = item;
#if RibbonGroupLayout_DEBUG_PRINT
        qDebug() << item->widget()->metaObject()->className() << " rp:" << rp << " row:" << item->rowIndex
                 << " col:" << item->columnIndex << " new row:" << row << " new column:" << column
                 << " itemWillSetGeometry:" << item->itemWillSetGeometry << " sizeHint:" << hint << " x:" << x;
#endif
    }
    // 最后一个元素，更新列数
    //  2022-06-20 此句本来在循环里面，如果最后一个元素隐藏，会导致无法到达此判断导致异常
    if (lastGeomItem) {   // 最后一个元素，更新totalWidth
        if (lastGeomItem->columnIndex != column) {
            // 说明最后一个元素处于最后位置，触发了换列，此时真实列数需要减1，直接等于column索引
            m_columnCount = column;
            // 由于最后一个元素触发了换列，x值是新一列的位置，直接作为totalWidth
            totalWidth = x + mag.right();
        } else {
            // 说明最后一个元素处于非最后位置，没有触发下一个换列，此时真实列数等于column索引+1
            m_columnCount = column + 1;
            // 由于最后一个元素未触发换列，需要计算totalWidth
            totalWidth = x + columnMaxWidth + spacing + mag.right();
        }
    }
    // 在有optionButton情况下，的2行模式，需要调整totalWidth
    if (group->isTwoRow()) {
        if (group->hasOptionAction()) {
            totalWidth += group->optionActionButtonSize().width();
        }
    }
    // 在设置完所有窗口后，再设置扩展属性的窗口
    if (totalWidth < setrect.width()) {
        // 说明可以设置扩展属性的窗口
        recalcExpandGeomArray(setrect);
    }
    m_sizeHint = QSize(totalWidth, height);
}

/**
 * @brief 布局所有action
 */
void RibbonGroupLayoutPrivate::layoutActions()
{
    if (m_dirty) {
        updateGeomArray(q->geometry());
    }
    QList<QWidget *> showWidgets, hideWidgets;

#if RibbonGroupLayout_DEBUG_PRINT
    qDebug() << "\r\n\r\n =============================================="
                "\r\n RibbonGroupLayout::layoutActions"
             << " \r\n name:" << q->parentWidget()->windowTitle() << " sizehint:" << q->sizeHint();
#endif
    for (RibbonGroupItem *item : qAsConst(m_items)) {
        if (item->isEmpty()) {
            hideWidgets << item->widget();
        } else {
            item->setGeometry(item->itemWillSetGeometry);
            // item->widget()->setFixedSize(item->itemWillSetGeometry.size());
            // item->widget()->move(item->itemWillSetGeometry.topLeft());
            showWidgets << item->widget();
#if RibbonGroupLayout_DEBUG_PRINT
            qDebug() << "[" << item->rowIndex << "," << item->columnIndex << "]"
                     << " -> " << item->itemWillSetGeometry << ":" << item->widget()->metaObject()->className();
#endif
        }
    }

    // 不在上面那里进行show和hide因为这会触发RibbonGroupLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget *w : qAsConst(showWidgets)) {
        w->show();
    }
    for (QWidget *w : qAsConst(hideWidgets)) {
        w->hide();
    }
}

RibbonGroupLayout::RibbonGroupLayout(QWidget *p)
    : QLayout(p)
    , d(new RibbonGroupLayoutPrivate)
{
    d->q = this;
    setSpacing(1);
}

RibbonGroupLayout::~RibbonGroupLayout()
{
    while (!d->m_items.isEmpty()) {
        RibbonGroupItem *item = d->m_items.takeFirst();
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(item->action)) {
            if (item->customWidget) {
                widgetAction->releaseWidget(item->widget());
            }
        }
        delete item;
    }
}

/**
 * @brief 通过action查找索引，用于actionEvent添加action用
 * @param action
 * @return 没有查到返回-1
 */
int RibbonGroupLayout::indexOf(QAction *action) const
{
    for (int i = 0; i < d->m_items.count(); ++i) {
        if (d->m_items.at(i)->action == action) {
            return i;
        }
    }
    return -1;
}

void RibbonGroupLayout::addItem(QLayoutItem *item)
{
    Q_UNUSED(item);
    qWarning("RibbonGroupLayout::addItem(): please use addAction() instead");
    return;
}

/**
 * @brief RibbonGroup主要通过此函数来添加action
 * @param act
 * @param rp 布局策略
 */
void RibbonGroupLayout::insertAction(int index, QAction *act, RibbonGroup::RowProportion rp)
{
    index = qMax(0, index);
    index = qMin(d->m_items.count(), index);
    RibbonGroupItem *item = d->createItem(act, rp);

    if (item) {
        d->m_items.insert(index, item);
        // 标记需要重新计算尺寸
        invalidate();
    }
}

QLayoutItem *RibbonGroupLayout::itemAt(int index) const
{
    if ((index < 0) || (index >= d->m_items.count())) {
        return Q_NULLPTR;
    }
    return d->m_items.at(index);
}

QLayoutItem *RibbonGroupLayout::takeAt(int index)
{
    if ((index < 0) || (index >= d->m_items.count())) {
        return Q_NULLPTR;
    }
    RibbonGroupItem *item = d->m_items.takeAt(index);

    QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(item->action);

    if ((widgetAction != 0) && item->customWidget) {
        widgetAction->releaseWidget(item->widget());
    } else {
        // destroy the QToolButton/QToolBarSeparator
        item->widget()->hide();
        item->widget()->deleteLater();
    }

    invalidate();
    return item;
}

int RibbonGroupLayout::count() const
{
    return d->m_items.count();
}

bool RibbonGroupLayout::isEmpty() const
{
    return d->m_items.isEmpty();
}

void RibbonGroupLayout::invalidate()
{
    d->m_dirty = true;
    QLayout::invalidate();
}

Qt::Orientations RibbonGroupLayout::expandingDirections() const
{
    return Qt::Horizontal;
}

QSize RibbonGroupLayout::minimumSize() const
{
    return d->m_sizeHint;
}

QSize RibbonGroupLayout::sizeHint() const
{
    return d->m_sizeHint;
}

/**
 * @brief 通过action获取RibbonGroupItem
 * @param action
 * @return 如果没有返回Q_NULLPTR
 */
RibbonGroupItem *RibbonGroupLayout::groupItem(QAction *action) const
{
    int index = indexOf(action);

    if (index >= 0) {
        return d->m_items[index];
    }
    return Q_NULLPTR;
}

/**
 * @brief 获取最后一个添加的item
 * @return 如果没有返回Q_NULLPTR
 */
RibbonGroupItem *RibbonGroupLayout::lastItem() const
{
    if (d->m_items.isEmpty()) {
        return Q_NULLPTR;
    }
    return d->m_items.last();
}

/**
 * @brief 获取最后生成的窗口
 * @return 如果无窗口或者item为空，返回Q_NULLPTR
 */
QWidget *RibbonGroupLayout::lastWidget() const
{
    RibbonGroupItem *item = lastItem();

    if (item) {
        return item->widget();
    }
    return Q_NULLPTR;
}

/**
 * @brief 移动两个item
 * @param from
 * @param to
 * @note 移动完后所有都失效，需要重新布局
 */
void RibbonGroupLayout::move(int from, int to)
{
    if (from == to) {
        return;
    }
    if (to < 0) {
        to = 0;
    }
    if (to >= count()) {
        to = count() - 1;
    }
    d->m_items.move(from, to);
    invalidate();
}

/**
 * @brief 判断是否需要重新布局
 * @return
 */
bool RibbonGroupLayout::isDirty() const
{
    return d->m_dirty;
}

/**
 * @brief 计算大图标的高度
 * @param setrect
 * @param group
 * @return
 */
int RibbonGroupLayout::calcLargeHeight(const QRect &setrect, const RibbonGroup *group)
{
    const QMargins &mag = groupContentsMargins();
    return setrect.height() - mag.top() - mag.bottom() - group->titleHeight();
}

/**
 * @brief 全局的contentsMargins
 *
 * 由于ribbonbar的高度估算需要预先知道group的contentsMargins，所有需要设置为全局，
 * 另外，设置为全局也是为了风格的统一
 *
 * @return
 */
const QMargins &RibbonGroupLayout::groupContentsMargins()
{
    return RibbonGroupLayoutPrivate::s_contentsMargins;
}

/**
 * @brief 全局的contentsMargins
 * @note RibbonStyleOption会用到此函数，调用设置函数后需要手动重新计算RibbonStyleOption的内容,@sa
 * RibbonStyleOption::recalc
 * @sa RibbonStyleOption
 * @param m
 */
void RibbonGroupLayout::setGroupContentsMargins(const QMargins &m)
{
    RibbonGroupLayoutPrivate::s_contentsMargins = m;
}

const QList<RibbonGroupItem *> &RibbonGroupLayout::ribbonGroupItems() const
{
    return d->m_items;
}

/**
 * @brief 返回所有列的区域
 * @return <列索引，列区域>
 */
// QMap<int, QRect> RibbonGroupLayout::columnsGeometry() const
//{
//    QMap<int, QRect> res;
//    for (RibbonGroupItem *item:m_items){
//        if(item->isEmpty()){
//           continue;
//        }
//        QMap<int, QRect>::iterator i = res.find(item->columnIndex);
//        if(i == res.end())
//        {
//            QRect r = item->geometry();
//            r.setY(this->contentsMargins().top());
//            r.setHeight(m_largeHeight);
//            i = res.insert(item->columnIndex,r);
//        }
//        if(item->itemWillSetGeometry.width() > i.value().width()){
//            i.value().setWidth(item->itemWillSetGeometry.width());
//        }
//    }
//    return res;
//}

void RibbonGroupLayout::setGeometry(const QRect &rect)
{
    d->m_dirty = false;
    d->updateGeomArray(rect);
    QLayout::setGeometry(rect);
    d->layoutActions();
}
