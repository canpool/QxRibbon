/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "QxRibbonGlobal.h"
#include <QMainWindow>

class RibbonWindowPrivate;
class RibbonBar;

/**
 * @brief 如果要使用 RibbonBar，必须使用此类代替 QMainWindow
 *
 * 由于 ribbon 的风格和传统的 MenuBar/Toolbar 风格差异较大，
 * RibbonBar 使用需要把原有的 QMainWindow 替换为 RibbonWindow,
 * RibbonWindow 是个无边框窗体，继承自 QMainWindow，其构造函数的参数 useRibbon
 * 用于指定是否使用 ribbon 风格，默认为 true
 *
 * @code
 * RibbonWindow(QWidget* parent = Q_NULLPTR,bool useRibbon = true);
 * @endcode
 *
 * 如果想换回非 ribbon 风格，只需要把 useRibbon 设置为 false 即可,
 * 成员函数 isUseRibbon 用于判断当前是否为 ribbon 模式，这个函数在兼容传统 Toolbar 风格和 ribbon 风格时非常有用。
 *
 * @code
 * bool isUseRibbon() const;
 * @endcode
 *
 * @ref RibbonWindow 提供了几种常用的 ribbon 样式，样式可见 @ref RibbonTheme
 * 通过 @ref setRibbonTheme 可改变 ribbon 的样式，用户也可通过 qss 自己定义自己的样式
 *
 */
class QX_RIBBON_EXPORT RibbonWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
public:
    enum RibbonTheme {
        NormalTheme,
        Office2013Theme,
        WpsdarkTheme,
        CustomTheme,
    };
    Q_ENUMS(RibbonTheme)
public:
    RibbonWindow(QWidget *parent = Q_NULLPTR, bool useRibbon = true);
    ~RibbonWindow() Q_DECL_OVERRIDE;

    const RibbonBar *ribbonBar() const;
    RibbonBar *ribbonBar();

    void setRibbonTheme(int theme);
    int ribbonTheme() const;

    bool isUseRibbon() const;

    bool isFrameless() const;
    void setFrameless(bool frameless);

    void updateWindowFlag(Qt::WindowFlags flags);
    Qt::WindowFlags windowButtonFlags() const;

    void setMenuWidget(QWidget *menuBar);
    void setMenuBar(QMenuBar *menuBar);
protected:
    void loadTheme(const QString &themeFile);
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
private:
    RibbonWindowPrivate *d;
};
