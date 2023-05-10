/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonGalleryGroup.h"
#include "QxRibbonManager.h"
#include "QxRibbonStyleOption.h"

#include <QActionGroup>
#include <QDebug>
#include <QItemSelectionModel>
#include <QPainter>

/* RibbonGalleryGroupPrivate */
class RibbonGalleryGroupPrivate
{
public:
    RibbonGalleryGroupPrivate(RibbonGalleryGroup *p);
public:
    RibbonGalleryGroup *q;
    QString m_groupTitle;
    RibbonGalleryGroup::GalleryGroupStyle m_preStyle;
    RibbonGalleryGroup::DisplayRow m_displayRow;
    int m_gridMinimumWidth;        ///< grid最小宽度
    int m_gridMaximumWidth;        ///< grid最大宽度
    QActionGroup *m_actionGroup;   ///< 所有GalleryGroup管理的actions都由这个actiongroup管理
    bool m_blockRecalc;
};

RibbonGalleryGroupPrivate::RibbonGalleryGroupPrivate(RibbonGalleryGroup *p)
    : q(p)
    , m_preStyle(RibbonGalleryGroup::IconWithText)
    , m_displayRow(RibbonGalleryGroup::DisplayOneRow)
    , m_gridMinimumWidth(0)
    , m_gridMaximumWidth(0)
    , m_blockRecalc(false)
{
    m_actionGroup = new QActionGroup(p);
    p->connect(m_actionGroup, &QActionGroup::triggered, p, &RibbonGalleryGroup::triggered);
    p->connect(m_actionGroup, &QActionGroup::hovered, p, &RibbonGalleryGroup::hovered);
}


/* RibbonGalleryGroupItemDelegate */
RibbonGalleryGroupItemDelegate::RibbonGalleryGroupItemDelegate(RibbonGalleryGroup *group, QObject *parent)
    : QStyledItemDelegate(parent), m_group(group)
{
}

void RibbonGalleryGroupItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    if (Q_NULLPTR == m_group) {
        return;
    }
    switch (m_group->getGalleryGroupStyle()) {
    case RibbonGalleryGroup::IconWithText:
        paintIconWithText(painter, option, index);
        break;
    case RibbonGalleryGroup::IconWithWordWrapText:
        paintIconWithTextWordWrap(painter, option, index);
        break;
    case RibbonGalleryGroup::IconOnly:
        paintIconOnly(painter, option, index);
        break;
    default:
        paintIconWithText(painter, option, index);
        break;
    }
}

void RibbonGalleryGroupItemDelegate::paintIconOnly(QPainter *painter, const QStyleOptionViewItem &option,
                                                    const QModelIndex &index) const
{
    QStyle *style = m_group->style();
    int sp = m_group->spacing();
    sp += 3;
    painter->save();
    painter->setClipRect(option.rect);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, m_group);
    // draw the icon
    QRect iconRect = option.rect;

    iconRect.adjust(sp, sp, -sp, -sp);
    QIcon ico = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

    ico.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    painter->restore();
}

void RibbonGalleryGroupItemDelegate::paintIconWithText(QPainter *painter, const QStyleOptionViewItem &option,
                                                        const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

void RibbonGalleryGroupItemDelegate::paintIconWithTextWordWrap(QPainter *painter, const QStyleOptionViewItem &option,
                                                                const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize RibbonGalleryGroupItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    return m_group->gridSize();
}

/* RibbonGalleryGroupModel */
RibbonGalleryGroupModel::RibbonGalleryGroupModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

RibbonGalleryGroupModel::~RibbonGalleryGroupModel()
{
    clear();
}

int RibbonGalleryGroupModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid() ? 0 : m_items.size());
}

Qt::ItemFlags RibbonGalleryGroupModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || (index.row() >= m_items.size())) {
        return Qt::NoItemFlags;
    }
    return m_items.at(index.row())->flags();
}

QVariant RibbonGalleryGroupModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return QVariant();
    }
    return m_items.at(index.row())->data(role);
}

QModelIndex RibbonGalleryGroupModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        return createIndex(row, column, m_items.at(row));
    }
    return QModelIndex();
}

bool RibbonGalleryGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return false;
    }

    m_items.at(index.row())->setData(role, value);
    return true;
}

void RibbonGalleryGroupModel::clear()
{
    beginResetModel();
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)) {
            delete m_items.at(i);
        }
    }
    m_items.clear();
    endResetModel();
}

RibbonGalleryItem *RibbonGalleryGroupModel::at(int row) const
{
    return m_items.value(row);
}

void RibbonGalleryGroupModel::insert(int row, RibbonGalleryItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(row, item);
    endInsertRows();
}

RibbonGalleryItem *RibbonGalleryGroupModel::take(int row)
{
    if ((row < 0) || (row >= m_items.count())) {
        return Q_NULLPTR;
    }

    beginRemoveRows(QModelIndex(), row, row);
    RibbonGalleryItem *item = m_items.takeAt(row);

    endRemoveRows();
    return item;
}

void RibbonGalleryGroupModel::append(RibbonGalleryItem *item)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count() + 1);
    m_items.append(item);
    endInsertRows();
}

/* RibbonGalleryGroup */
RibbonGalleryGroup::RibbonGalleryGroup(QWidget *w)
    : QListView(w)
    , d(new RibbonGalleryGroupPrivate(this))
{
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setSelectionRectVisible(true);
    setUniformItemSizes(true);
    setSpacing(1);
    setItemDelegate(new RibbonGalleryGroupItemDelegate(this, this));
    connect(this, &QAbstractItemView::clicked, this, &RibbonGalleryGroup::onItemClicked);
    RibbonGalleryGroupModel *m = new RibbonGalleryGroupModel(this);
    setModel(m);
}

RibbonGalleryGroup::~RibbonGalleryGroup()
{
    delete d;
}

/**
 * @brief 是否禁止计算
 * @param on
 */
void RibbonGalleryGroup::setRecalcGridSizeBlock(bool on)
{
    d->m_blockRecalc = on;
}

bool RibbonGalleryGroup::isRecalcGridSizeBlock() const
{
    return d->m_blockRecalc;
}

/**
 * @brief 重新计算grid和icon的尺寸
 */
void RibbonGalleryGroup::recalcGridSize()
{
    recalcGridSize(height());
}

void RibbonGalleryGroup::recalcGridSize(int galleryHeight)
{
    if (isRecalcGridSizeBlock()) {
        return;
    }
    // 首先通过DisplayRow计算GridSize
    int dr = static_cast<int>(getDisplayRow());
    if (dr < 1) {
        dr = 1;
    } else if (dr > 3) {
        dr = 3;
    }
    int h = galleryHeight / dr;
    if (h <= 1) {
        h = galleryHeight;
    }
    int w = h;
    if (getGridMinimumWidth() > 0) {
        if (w < getGridMinimumWidth()) {
            w = getGridMinimumWidth();
        }
    }
    if (getGridMaximumWidth() > 0) {
        if (w > getGridMaximumWidth()) {
            w = getGridMaximumWidth();
        }
    }
    setGridSize(QSize(w, h));
    // 在通过GalleryGroupStyle确定icon的尺寸
    // 这个是移动像素，qt在鼠标移动到图标上时会移动一下，给用户明确的动态，导致如果布局很满会超出显示范围，因此要在此基础上缩放一点
    const int shiftpix = 4;
    switch (getGalleryGroupStyle()) {
    case IconWithText: {
        int textHeight = fontMetrics().lineSpacing();
        int iconHeight = h - textHeight - 2 * spacing() - shiftpix;
        if (iconHeight > 0) {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, iconHeight));
        } else {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        }
        break;
    }
    case IconWithWordWrapText: {
        int textHeight = fontMetrics().lineSpacing() * 2;
        int iconHeight = h - textHeight;
        if (iconHeight > 0) {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, iconHeight - 2 * spacing() - shiftpix));
        } else {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        }
        break;
    }
    case IconOnly: {
        setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        break;
    }
    default: {
        setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        break;
    }
    }
#if 0
    qDebug() << "RibbonGalleryGroup::recalcGridSize(" << galleryHeight << "): gridSize=" << gridSize()
             << " iconSize=" << iconSize();
#endif
}

///
/// \brief 设置默认的预设样式
/// \param style
///
void RibbonGalleryGroup::setGalleryGroupStyle(RibbonGalleryGroup::GalleryGroupStyle style)
{
    d->m_preStyle = style;
    if (style == IconWithWordWrapText) {
        setWordWrap(true);
    }
    recalcGridSize();
}

RibbonGalleryGroup::GalleryGroupStyle RibbonGalleryGroup::getGalleryGroupStyle() const
{
    return d->m_preStyle;
}

void RibbonGalleryGroup::addItem(const QString &text, const QIcon &icon)
{
    if (Q_NULLPTR == groupModel()) {
        return;
    }
    addItem(new RibbonGalleryItem(text, icon));
}

/**
 * @brief 添加一个条目
 *
 * @param item 条目的内存所有权归属RibbonGalleryGroup管理
 */
void RibbonGalleryGroup::addItem(RibbonGalleryItem *item)
{
    if (Q_NULLPTR == groupModel()) {
        return;
    }
    groupModel()->append(item);
}

void RibbonGalleryGroup::addActionItem(QAction *act)
{
    if (Q_NULLPTR == groupModel()) {
        return;
    }
    d->m_actionGroup->addAction(act);
    groupModel()->append(new RibbonGalleryItem(act));
}

void RibbonGalleryGroup::addActionItemList(const QList<QAction *> &acts)
{
    RibbonGalleryGroupModel *model = groupModel();

    if (Q_NULLPTR == model) {
        return;
    }
    for (QAction *a : acts) {
        d->m_actionGroup->addAction(a);
    }
    for (int i = 0; i < acts.size(); ++i) {
        model->append(new RibbonGalleryItem(acts[i]));
    }
}

///
/// \brief 构建一个model，这个model的父类是RibbonGalleryGroup，如果要共享model，需要手动处理model的父类
///
void RibbonGalleryGroup::setupGroupModel()
{
    setModel(new RibbonGalleryGroupModel(this));
}

RibbonGalleryGroupModel *RibbonGalleryGroup::groupModel()
{
    return qobject_cast<RibbonGalleryGroupModel *>(model());
}

void RibbonGalleryGroup::setGroupTitle(const QString &title)
{
    d->m_groupTitle = title;
    emit groupTitleChanged(d->m_groupTitle);
}

QString RibbonGalleryGroup::getGroupTitle() const
{
    return d->m_groupTitle;
}

void RibbonGalleryGroup::selectByIndex(int i)
{
    RibbonGalleryGroupModel *model = groupModel();

    if (Q_NULLPTR == model) {
        return;
    }
    QModelIndex ind = model->index(i, 0, QModelIndex());
    QItemSelectionModel *selmodel = selectionModel();

    if (selmodel) {
        selmodel->select(ind, QItemSelectionModel::SelectCurrent);
    }
}

/**
 * @brief 设置显示的行数
 * @param r
 */
void RibbonGalleryGroup::setDisplayRow(DisplayRow r)
{
    d->m_displayRow = r;
    recalcGridSize();
}

/**
 * @brief Gallery显示的行数
 * @return
 */
RibbonGalleryGroup::DisplayRow RibbonGalleryGroup::getDisplayRow() const
{
    return d->m_displayRow;
}

/**
 * @brief 设置grid最小的宽度，默认为0（不限制）
 * @param w
 */
void RibbonGalleryGroup::setGridMinimumWidth(int w)
{
    d->m_gridMinimumWidth = w;
}

/**
 * @brief grid最小的宽度，默认为0（不限制）
 * @return
 */
int RibbonGalleryGroup::getGridMinimumWidth() const
{
    return d->m_gridMinimumWidth;
}

/**
 * @brief 设置grid最大的宽度，默认为0（不限制）
 * @param w
 */
void RibbonGalleryGroup::setGridMaximumWidth(int w)
{
    d->m_gridMaximumWidth = w;
}

/**
 * @brief grid最大的的宽度，默认为0（不限制）
 * @param w
 */
int RibbonGalleryGroup::getGridMaximumWidth() const
{
    return d->m_gridMaximumWidth;
}

/**
 * @brief 获取RibbonGalleryGroup管理的actiongroup
 * @return
 */
QActionGroup *RibbonGalleryGroup::getActionGroup() const
{
    return d->m_actionGroup;
}

void RibbonGalleryGroup::onItemClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        RibbonGalleryItem *item = (RibbonGalleryItem *)index.internalPointer();
        if (item) {
            QAction *act = item->action();
            if (act) {
                act->activate(QAction::Trigger);
            }
        }
    }
}

void RibbonGalleryGroup::onItemEntered(const QModelIndex &index)
{
    if (index.isValid()) {
        RibbonGalleryItem *item = (RibbonGalleryItem *)index.internalPointer();
        if (item) {
            QAction *act = item->action();
            if (act) {
                act->activate(QAction::Hover);
            }
        }
    }
}
