/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include "QxRibbonCustomizeWidget.h"
#include <QDialog>

class QXmlStreamWriter;

class RibbonActionsManager;
class RibbonCustomizeDialogUi;

/**
 * @brief RibbonCustomizeWidget 的对话框封装
 *
 * 此功能依赖于 @ref RibbonActionsManager ，RibbonActionsManager 建议作为 mianwindow 的成员变量，
 * RibbonActionsManager 可以快速绑定所有 QAction，详细见 RibbonActionsManager 的说明
 *
 * @note Ribbon 的自定义是基于步骤的，如果在窗口生成前调用了 @ref ribbon_apply_customize_from_xml_file 类似函数
 * 那么在对话框生成前为了保证同步需要调用 @ref RibbonCustomizeDialog::fromXml
 * 同步配置文件，这样再次修改后的配置文件就一致
 */
class QX_RIBBON_EXPORT RibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    RibbonCustomizeDialog(RibbonWindow *ribbonWindow, QWidget *p = Q_NULLPTR,
                           Qt::WindowFlags f = Qt::WindowFlags());

    // 设置action管理器
    void setupActionsManager(RibbonActionsManager *mgr);

    // 应用所有的设定
    bool applys();

    // 清除所有动作
    void clear();

    // 转换为xml
    bool toXml(QXmlStreamWriter *xml) const;
    bool toXml(const QString &xmlpath) const;

    // 从xml中加载QList<RibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
    void fromXml(QXmlStreamReader *xml);
    void fromXml(const QString &xmlpath);

    // 返回RibbonCustomizeWidget窗口指针
    RibbonCustomizeWidget *customizeWidget() const;
private:
    void initConnection();

    RibbonCustomizeDialogUi *ui;
};
