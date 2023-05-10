/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonCustomizeData.h"
#include "QxRibbonBar.h"
#include "QxRibbonWindow.h"

#include <QDebug>
#include <QObject>

/**
 * @def 属性，用于标记是否可以进行自定义，用于动态设置到 @ref RibbonPage 和 @ref RibbonGroup
 * 值为bool，在为true时，可以通过 @ref RibbonCustomizeWidget 改变这个RibbonPage和RibbonGroup的布局，
 * 默认不会有此属性，仅在有此属性且为true时才会在RibbonCustomizeWidget中能显示为可设置
 */
#ifndef QX_RIBBON_PROP_CAN_CUSTOMIZE
#define QX_RIBBON_PROP_CAN_CUSTOMIZE "_qx_isCanCustomize"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RibbonCustomizeData
////////////////////////////////////////////////////////////////////////////////////////////////////////

RibbonCustomizeData::RibbonCustomizeData()
    : indexValue(-1)
    , actionRowProportionValue(RibbonGroup::Large)
    , m_type(UnknowActionType)
    , m_actionsManagerPointer(Q_NULLPTR)
{
}

RibbonCustomizeData::RibbonCustomizeData(ActionType type, RibbonActionsManager *mgr)
    : indexValue(-1)
    , actionRowProportionValue(RibbonGroup::Large)
    , m_type(type)
    , m_actionsManagerPointer(mgr)
{
}

/**
 * @brief 获取CustomizeData的action type
 * @return
 */
RibbonCustomizeData::ActionType RibbonCustomizeData::actionType() const
{
    return m_type;
}

/**
 * @brief 设置CustomizeData的action type
 * @param a
 */
void RibbonCustomizeData::setActionType(RibbonCustomizeData::ActionType a)
{
    m_type = a;
}

/**
 * @brief 判断是否是一个正常的CustomizeData
 *
 * 实际逻辑actionType() != UnknowActionType
 * @return 有用的CustomizeData返回true
 */
bool RibbonCustomizeData::isValid() const
{
    return (actionType() != UnknowActionType);
}

/**
 * @brief 应用RibbonCustomizeData到RibbonWindow
 * @param m
 * @return 如果应用失败，返回false,如果actionType==UnknowActionType直接返回false
 */
bool RibbonCustomizeData::apply(RibbonWindow *m) const
{
    RibbonBar *bar = m->ribbonBar();

    if (Q_NULLPTR == bar) {
        return false;
    }
    switch (actionType()) {
    case UnknowActionType:
        return false;

    case AddPageActionType: {
        // 添加标签
        RibbonPage *c = bar->insertPage(indexValue, keyValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        c->setObjectName(pageObjNameValue);
        RibbonCustomizeData::setCanCustomize(c);
        return true;
    }

    case AddGroupActionType: {
        // 添加group
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        RibbonGroup *p = c->insertGroup(indexValue, keyValue);
        p->setObjectName(groupObjNameValue);
        RibbonCustomizeData::setCanCustomize(p);
        return true;
    }

    case AddActionActionType: {
        if (Q_NULLPTR == m_actionsManagerPointer) {
            return false;
        }
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        RibbonGroup *group = c->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        QAction *act = m_actionsManagerPointer->action(keyValue);
        if (Q_NULLPTR == act) {
            return false;
        }
        RibbonCustomizeData::setCanCustomize(act);
        group->addAction(act, actionRowProportionValue);
        return true;
    }

    case RemovePageActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        bar->removePage(c);
        return true;
    }

    case RemoveGroupActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        RibbonGroup *group = c->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        c->removeGroup(group);
        return true;
    }

    case RemoveActionActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        RibbonGroup *group = c->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        QAction *act = m_actionsManagerPointer->action(keyValue);
        if (Q_NULLPTR == act) {
            return false;
        }
        group->removeAction(act);
        return true;
    }

    case ChangePageOrderActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        int currentindex = bar->pageIndex(c);
        if (-1 == currentindex) {
            return false;
        }
        int toindex = currentindex + indexValue;
        bar->movePage(currentindex, toindex);
        return true;
    }

    case ChangeGroupOrderActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        RibbonGroup *group = c->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        int groupIndex = c->groupIndex(group);
        if (-1 == groupIndex) {
            return false;
        }
        c->moveGroup(groupIndex, groupIndex + indexValue);
        return true;
    }

    case ChangeActionOrderActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        RibbonGroup *group = c->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        QAction *act = m_actionsManagerPointer->action(keyValue);
        if (Q_NULLPTR == act) {
            return false;
        }
        int actindex = group->actionIndex(act);
        if (actindex <= -1) {
            return false;
        }
        group->moveAction(actindex, actindex + indexValue);
        return true;
    }

    case RenamePageActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        c->setWindowTitle(keyValue);
        return true;
    }

    case RenameGroupActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        RibbonGroup *group = c->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        group->setWindowTitle(keyValue);
        return true;
    }

    case VisiblePageActionType: {
        RibbonPage *c = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == c) {
            return false;
        }
        if (1 == indexValue) {
            bar->showPage(c);
        } else {
            bar->hidePage(c);
        }
        return true;
    }

    default:
        break;
    }
    return false;
}

/**
 * @brief 获取actionmanager指针
 * @return
 */
RibbonActionsManager *RibbonCustomizeData::actionManager()
{
    return m_actionsManagerPointer;
}

/**
 * @brief 设置ActionsManager
 * @param mgr
 */
void RibbonCustomizeData::setActionsManager(RibbonActionsManager *mgr)
{
    m_actionsManagerPointer = mgr;
}

/**
 * @brief 创建一个AddPageActionType的RibbonCustomizeData
 * @param title page 的标题
 * @param index page要插入的位置
 * @param objName page的object name
 * @return 返回AddPageActionType的RibbonCustomizeData
 */
RibbonCustomizeData RibbonCustomizeData::makeAddPageCustomizeData(const QString &title, int index,
                                                                        const QString &objName)
{
    RibbonCustomizeData data(AddPageActionType);

    data.indexValue = index;
    data.keyValue = title;
    data.pageObjNameValue = objName;
    return data;
}

/**
 * @brief 创建一个AddGroupActionType的RibbonCustomizeData
 * @param title group的标题
 * @param index group的index
 * @param pageobjName group的page的objectname
 * @param objName group的objname
 * @return 返回AddGroupActionType的RibbonCustomizeData
 */
RibbonCustomizeData RibbonCustomizeData::makeAddGroupCustomizeData(const QString &title, int index,
                                                                      const QString &pageobjName,
                                                                      const QString &objName)
{
    RibbonCustomizeData data(AddGroupActionType);

    data.indexValue = index;
    data.keyValue = title;
    data.groupObjNameValue = objName;
    data.pageObjNameValue = pageobjName;
    return data;
}

/**
 * @brief 添加action
 * @param key action的索引
 * @param mgr action管理器
 * @param rp 定义action的占位情况
 * @param pageObjName action添加到的page的objname
 * @param groupObjName action添加到的page下的group的objname
 * @param index action添加到的group的索引
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeAddActionCustomizeData(const QString &key, RibbonActionsManager *mgr,
                                                                      RibbonGroup::RowProportion rp,
                                                                      const QString &pageObjName,
                                                                      const QString &groupObjName)
{
    RibbonCustomizeData data(AddActionActionType, mgr);

    data.keyValue = key;
    data.pageObjNameValue = pageObjName;
    data.groupObjNameValue = groupObjName;
    data.actionRowProportionValue = rp;

    return data;
}

/**
 * @brief 创建一个RenamePageActionType的RibbonCustomizeData
 * @param newname 新名字
 * @param index page的索引
 * @return 返回RenamePageActionType的RibbonCustomizeData
 */
RibbonCustomizeData RibbonCustomizeData::makeRenamePageCustomizeData(const QString &newname,
                                                                           const QString &pageobjName)
{
    RibbonCustomizeData data(RenamePageActionType);

    if (pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize rename page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.keyValue = newname;
    data.pageObjNameValue = pageobjName;
    return data;
}

/**
 * @brief 创建一个RenameGroupActionType的RibbonCustomizeData
 * @param newname group的名字
 * @param indexValue group的索引
 * @param pageobjName group对应的page的object name
 * @return 返回RenameGroupActionType的RibbonCustomizeData
 */
RibbonCustomizeData RibbonCustomizeData::makeRenameGroupCustomizeData(const QString &newname,
                                                                         const QString &pageobjName,
                                                                         const QString &groupObjName)
{
    RibbonCustomizeData data(RenameGroupActionType);

    if (groupObjName.isEmpty() || pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize rename group,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.keyValue = newname;
    data.groupObjNameValue = groupObjName;
    data.pageObjNameValue = pageobjName;
    return data;
}

/**
 * @brief 对应ChangePageOrderActionType
 * @param pageobjName 需要移动的pageobjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangePageOrderCustomizeData(const QString &pageobjName,
                                                                                int moveindex)
{
    RibbonCustomizeData data(ChangePageOrderActionType);

    if (pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change page order,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.indexValue = moveindex;
    return data;
}

/**
 * @brief 对应ChangeGroupOrderActionType
 * @param pageobjName 需要移动的group对应的pageobjName
 * @param groupObjName 需要移动的groupObjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangeGroupOrderCustomizeData(const QString &pageobjName,
                                                                              const QString &groupObjName,
                                                                              int moveindex)
{
    RibbonCustomizeData data(ChangeGroupOrderActionType);

    if (pageobjName.isEmpty() || groupObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change group order,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.groupObjNameValue = groupObjName;
    data.indexValue = moveindex;
    return data;
}

/**
 * @brief 对应ChangeActionOrderActionType
 * @param pageobjName 需要移动的group对应的pageobjName
 * @param groupObjName 需要移动的groupObjName
 * @param key RibbonActionsManager管理的key名
 * @param mgr RibbonActionsManager指针
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangeActionOrderCustomizeData(const QString &pageobjName,
                                                                              const QString &groupObjName,
                                                                              const QString &key,
                                                                              RibbonActionsManager *mgr, int moveindex)
{
    RibbonCustomizeData data(ChangeActionOrderActionType, mgr);

    if (pageobjName.isEmpty() || groupObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change action order,"
                                "but get an empty page/group/action object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.groupObjNameValue = groupObjName;
    data.keyValue = key;
    data.indexValue = moveindex;
    return data;
}

/**
 * @brief 对应RemovePageActionType
 * @param pageobjName 需要移除的objname
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeRemovePageCustomizeData(const QString &pageobjName)
{
    RibbonCustomizeData data(RemovePageActionType);

    if (pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    return data;
}

/**
 * @brief 对应RemoveGroupActionType
 * @param pageobjName group对应的page的obj name
 * @param groupObjName group对应的 obj name
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeRemoveGroupCustomizeData(const QString &pageobjName,
                                                                         const QString &groupObjName)
{
    RibbonCustomizeData data(RemoveGroupActionType);

    if (pageobjName.isEmpty() || groupObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove group,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.groupObjNameValue = groupObjName;
    return data;
}

/**
 * @brief 对应RemoveActionActionType
 * @param pageobjName group对应的page的obj name
 * @param groupObjName group对应的 obj name
 * @param key RibbonActionsManager管理的key名
 * @param mgr RibbonActionsManager指针
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeRemoveActionCustomizeData(const QString &pageobjName,
                                                                         const QString &groupObjName,
                                                                         const QString &key, RibbonActionsManager *mgr)
{
    RibbonCustomizeData data(RemoveActionActionType, mgr);

    if (pageobjName.isEmpty() || groupObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove action,"
                                "but get an empty page/group/action object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.groupObjNameValue = groupObjName;
    data.keyValue = key;
    return data;
}

/**
 * @brief RibbonCustomizeData::makeVisiblePageCustomizeData
 * @param pageobjName
 * @param isShow
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeVisiblePageCustomizeData(const QString &pageobjName, bool isShow)
{
    RibbonCustomizeData data(VisiblePageActionType);

    if (pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize visible page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.indexValue = isShow ? 1 : 0;
    return data;
}

/**
 * @brief 判断外置属性，是否允许自定义
 * @param obj
 * @return
 */
bool RibbonCustomizeData::isCanCustomize(QObject *obj)
{
    QVariant v = obj->property(QX_RIBBON_PROP_CAN_CUSTOMIZE);

    if (v.isValid()) {
        return v.toBool();
    }
    return false;
}

/**
 * @brief 设置外置属性允许自定义
 * @param obj
 * @param canbe
 */
void RibbonCustomizeData::setCanCustomize(QObject *obj, bool canbe)
{
    obj->setProperty(QX_RIBBON_PROP_CAN_CUSTOMIZE, canbe);
}

QList<RibbonCustomizeData> remove_indexs(const QList<RibbonCustomizeData> &csd, const QList<int> &willremoveIndex);

QList<RibbonCustomizeData> remove_indexs(const QList<RibbonCustomizeData> &csd, const QList<int> &willremoveIndex)
{
    QList<RibbonCustomizeData> res;

    for (int i = 0; i < csd.size(); ++i) {
        if (!willremoveIndex.contains(i)) {
            res << csd[i];
        }
    }
    return res;
}

/**
 * @brief 对QList<RibbonCustomizeData>进行简化操作
 *
 * 此函数会执行如下操作：
 * 1、针对同一个page/group连续出现的添加和删除操作进行移除（前一步添加，后一步删除）
 *
 * 2、针对VisiblePageActionType，对于连续出现的操作只保留最后一步
 *
 * 3、针对RenamePageActionType和RenameGroupActionType操作，只保留最后一个
 *
 * 4、针对连续的ChangePageOrderActionType，ChangeGroupOrderActionType，ChangeActionOrderActionType进行合并为一个动作，
 * 如果合并后原地不动，则删除
 *
 * @param csd
 * @return 返回简化的QList<RibbonCustomizeData>
 */
QList<RibbonCustomizeData> RibbonCustomizeData::simplify(const QList<RibbonCustomizeData> &csd)
{
    int size = csd.size();

    if (size <= 1) {
        return csd;
    }
    QList<RibbonCustomizeData> res;
    QList<int> willremoveIndex;   // 记录要删除的index

    //! 首先针对连续出现的添加和删除操作进行优化
    for (int i = 1; i < size; ++i) {
        if ((csd[i - 1].actionType() == AddPageActionType) && (csd[i].actionType() == RemovePageActionType)) {
            if (csd[i - 1].pageObjNameValue == csd[i].pageObjNameValue) {
                willremoveIndex << i - 1 << i;
            }
        } else if ((csd[i - 1].actionType() == AddGroupActionType) &&
                   (csd[i].actionType() == RemoveGroupActionType)) {
            if ((csd[i - 1].groupObjNameValue == csd[i].groupObjNameValue) &&
                (csd[i - 1].pageObjNameValue == csd[i].pageObjNameValue)) {
                willremoveIndex << i - 1 << i;
            }
        } else if ((csd[i - 1].actionType() == AddActionActionType) &&
                   (csd[i].actionType() == RemoveActionActionType)) {
            if ((csd[i - 1].keyValue == csd[i].keyValue) &&
                (csd[i - 1].groupObjNameValue == csd[i].groupObjNameValue) &&
                (csd[i - 1].pageObjNameValue == csd[i].pageObjNameValue)) {
                willremoveIndex << i - 1 << i;
            }
        }
    }
    res = remove_indexs(csd, willremoveIndex);
    willremoveIndex.clear();

    //! 筛选VisiblePageActionType，对于连续出现的操作只保留最后一步
    size = res.size();
    for (int i = 1; i < size; ++i) {
        if ((res[i - 1].actionType() == VisiblePageActionType) &&
            (res[i].actionType() == VisiblePageActionType)) {
            if (res[i - 1].pageObjNameValue == res[i].pageObjNameValue) {
                // 要保证操作的是同一个内容
                willremoveIndex << i - 1;   // 删除前一个只保留最后一个
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 针对RenamePageActionType和RenameGroupActionType操作，只需保留最后一个
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if (res[i].actionType() == RenamePageActionType) {
            // 向后查询，如果查询到有同一个Page改名，把这个索引加入删除队列
            for (int j = i + 1; j < size; ++j) {
                if ((res[j].actionType() == RenamePageActionType) &&
                    (res[i].pageObjNameValue == res[j].pageObjNameValue)) {
                    willremoveIndex << i;
                }
            }
        } else if (res[i].actionType() == RenameGroupActionType) {
            // 向后查询，如果查询到有同一个group改名，把这个索引加入删除队列
            for (int j = i + 1; j < size; ++j) {
                if ((res[j].actionType() == RenameGroupActionType) &&
                    (res[i].groupObjNameValue == res[j].groupObjNameValue) &&
                    (res[i].pageObjNameValue == res[j].pageObjNameValue)) {
                    willremoveIndex << i;
                }
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 针对连续的ChangePageOrderActionType，ChangeGroupOrderActionType，ChangeActionOrderActionType进行合并
    size = res.size();
    for (int i = 1; i < size; ++i) {
        if ((res[i - 1].actionType() == ChangePageOrderActionType) &&
            (res[i].actionType() == ChangePageOrderActionType) &&
            (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i - 1].indexValue;
            willremoveIndex << i - 1;
        } else if ((res[i - 1].actionType() == ChangeGroupOrderActionType) &&
                   (res[i].actionType() == ChangeGroupOrderActionType) &&
                   (res[i - 1].groupObjNameValue == res[i].groupObjNameValue) &&
                   (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i - 1].indexValue;
            willremoveIndex << i - 1;
        } else if ((res[i - 1].actionType() == ChangeActionOrderActionType) &&
                   (res[i].actionType() == ChangeActionOrderActionType) && (res[i - 1].keyValue == res[i].keyValue) &&
                   (res[i - 1].groupObjNameValue == res[i].groupObjNameValue) &&
                   (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i - 1].indexValue;
            willremoveIndex << i - 1;
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 上一步操作可能会产生indexvalue为0的情况，此操作把indexvalue为0的删除
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if ((res[i].actionType() == ChangePageOrderActionType) ||
            (res[i].actionType() == ChangeGroupOrderActionType) ||
            (res[i].actionType() == ChangeActionOrderActionType)) {
            if (0 == res[i].indexValue) {
                willremoveIndex << i;
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();
    return res;
}
