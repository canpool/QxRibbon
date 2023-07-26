/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonButton.h"
#include "QxRibbonGroup.h"

#include <QCursor>
#include <QAction>
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QStyleOptionFocusRect>
#include <QStyleOptionToolButton>
#include <QStylePainter>

#define QX_INDICATOR_ARROW_WIDTH             8
#define QX_WIDTH_HEIGHT_RATIO                1.2

/**
 * @def 开启此宏会打印一些常见信息
 */
#define DebugRibbonButton_TextDrawPrint 0

// #define QX_RIBBON_DEBUG_HELP_DRAW

#ifdef QX_RIBBON_DEBUG_HELP_DRAW
    #define HELP_DRAW_RECT(p, rect)                                                                                    \
        do {                                                                                                           \
            p.save();                                                                                                  \
            p.setPen(Qt::red);                                                                                         \
            p.setBrush(QBrush());                                                                                      \
            p.drawRect(rect);                                                                                          \
            p.restore();                                                                                               \
        } while (0)

QDebug operator<<(QDebug debug, const QStyleOptionToolButton &opt)
{
    debug << "=============="
          << "\nQStyleOption(" << (QStyleOption)opt << ")"
          << "\n  QStyleOptionComplex:"
             "\n     subControls("
          << opt.subControls
          << " ) "
             "\n     activeSubControls("
          << opt.activeSubControls
          << "\n  QStyleOptionToolButton"
             "\n     features("
          << opt.features
          << ")"
             "\n     toolButtonStyle("
          << opt.toolButtonStyle << ")";

    return (debug);
}

#endif

class RibbonButtonPrivate
{
public:
    RibbonButtonPrivate();

    QStyle *style() const;

    void recalcSizeHint(QStyleOptionToolButton &opt, QSize s);
    // 此函数用来计算icon和text的尺寸
    // 注意icon的尺寸依赖text，且只要知道字体大小就可以确定icon的高度
    void calcIconAndTextRect(const QStyleOptionToolButton &opt);
    // 在需要绘制文字的时候调用此函数，有些效果需要文字的文字改变
    QRect adjustedTextRect(const QStyleOptionToolButton &opt, const QWidget *w);
    QRect calcIndicatorArrowDownRect(const QStyleOptionToolButton &opt);
    QPixmap createIconPixmap(const QStyleOptionToolButton &opt, const QSize &iconsize);

    void drawButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w);
    void drawLargeButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w);
    void drawSmallButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w);
    void drawIconAndLabel(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w);
    void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton, const QRect &rect,
                   QPainter *painter, const QWidget *widget = Q_NULLPTR);

public:
    RibbonButton *q;
    RibbonButton::RibbonButtonType m_buttonType;
    RibbonButton::LargeButtonType m_largeButtonType;
    bool m_mouseOnSubControl;   ///< 这个用于标记MenuButtonPopup模式下，鼠标在文本区域
    bool m_menuButtonPressed;
    bool m_isWordWrap;          ///< 标记是否文字换行 @default false
    int m_iconAndTextSpace;     ///< 按钮和边框的距离
    QSize m_sizeHint;           ///< 保存计算好的sizehint
    QRect m_iconRect;           ///< 记录icon的绘制位置
    QRect m_textRect;           ///< 记录text的绘制位置

    static bool s_isToolButtonTextShift;   ///< 配置RibbonButton文字在点击时是否会轻微改变位置而达到一种类似跳动的效果,
                                           ///< @default 默认为false
    static bool s_liteStyleEnableWordWrap; ///< 在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
};

bool RibbonButtonPrivate::s_isToolButtonTextShift = false;
bool RibbonButtonPrivate::s_liteStyleEnableWordWrap = false;

RibbonButtonPrivate::RibbonButtonPrivate()
    : m_buttonType(RibbonButton::LargeButton)
    , m_largeButtonType(RibbonButton::Normal)
    , m_mouseOnSubControl(false)
    , m_menuButtonPressed(false)
    , m_isWordWrap(false)
    , m_iconAndTextSpace(2)
{

}

QStyle *RibbonButtonPrivate::style() const
{
    return q->style();
}

void RibbonButtonPrivate::drawSmallButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w)
{
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        if (!w->rect().contains(w->mapFromGlobal(QCursor::pos()))) {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
    QStyle::State mflags = bflags;

    if (autoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
            mflags |= QStyle::State_MouseOver | QStyle::State_Sunken;
        } else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            mflags |= QStyle::State_Sunken;
            bflags |= QStyle::State_MouseOver;
        }
    }
    // 绘制背景
    QStyleOption tool(0);
    tool.palette = opt.palette;
    QStyle::PrimitiveElement pe = autoRaise ? QStyle::PE_PanelButtonTool : QStyle::PE_PanelButtonBevel;
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.activeSubControls &= QStyle::SC_ToolButtonMenu) {
            // 菜单激活,整个按钮都绘制为选中
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, w);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, w);
            if (tool.state & QStyle::State_MouseOver) {
                if (m_mouseOnSubControl) {   // 此时鼠标在indecater那
                    // 鼠标在文字区，把图标显示为正常
                    tool.rect = m_iconRect;
                    tool.state = (QStyle::State_Raised);   // 把图标区域显示为正常
                } else {
                    // 鼠标在图标区，把文字显示为正常
                    tool.state = (QStyle::State_Raised);   // 把图标区域显示为正常
                    tool.rect = opt.rect.adjusted(m_iconRect.width() + m_iconAndTextSpace, m_iconAndTextSpace,
                                                  -m_iconAndTextSpace, -m_iconAndTextSpace);
                }
                style()->drawPrimitive(pe, &tool, &p, w);
            }
        }
    } else if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::HasMenu)) {
        tool.rect = opt.rect;
        tool.state = bflags;
        style()->drawPrimitive(pe, &tool, &p, w);
    } else if (opt.subControls & QStyle::SC_ToolButton) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        style()->drawPrimitive(pe, &tool, &p, w);
    }
    drawIconAndLabel(opt, p, w);
}

void RibbonButtonPrivate::drawLargeButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w)
{
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        if (!w->rect().contains(w->mapFromGlobal(QCursor::pos()))) {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }

    bool autoRaise = opt.state & QStyle::State_AutoRaise;
#if 0
    QStyle::State bflags = opt.state;
#else
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
#endif

    if (autoRaise) {
        // 如果autoRaise，但鼠标不在按钮上或者按钮不是激活状态，去除raised状态
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }

    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
        } else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            bflags |= QStyle::State_MouseOver;
        }
    }

    // 绘制背景
    QStyleOption tool(0);
    tool.palette = opt.palette;
    QStyle::PrimitiveElement pe = autoRaise ? QStyle::PE_PanelButtonTool : QStyle::PE_PanelButtonBevel;
    // MenuButtonPopup特殊处理
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        // 此时按钮的菜单弹出
        tool.rect = opt.rect;
        tool.state = bflags;
        // 先把整个按钮绘制为选中
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, w);
        if (opt.activeSubControls &= QStyle::SC_ToolButtonMenu) {
            // 菜单激活 不做处理
        } else {
            // 菜单没有激活,这时候要把图标域或者文字域绘制为正常模式
            //  style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (tool.state & QStyle::State_MouseOver) {
                if (m_mouseOnSubControl) {   // 此时鼠标在indecater那
                    // 鼠标在文字区，把图标显示为正常
                    tool.rect = m_iconRect;
                    tool.state = (QStyle::State_Raised);   // 把图标区域显示为正常
                } else {
                    // 鼠标在图标区，把文字显示为正常
                    tool.state = (QStyle::State_Raised);   // 把图标区域显示为正常
                    tool.rect.setRect(m_textRect.x(), m_textRect.y(), tool.rect.width() - 2 * m_iconAndTextSpace,
                                      m_textRect.height());
                }
                style()->drawPrimitive(pe, &tool, &p, w);
            }
        }
    } else if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::HasMenu)) {
        // 按钮含有菜单
        tool.rect = opt.rect;
        tool.state = bflags;
        style()->drawPrimitive(pe, &tool, &p, w);
    } else if (opt.subControls & QStyle::SC_ToolButton) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        style()->drawPrimitive(pe, &tool, &p, w);
    }

    // 绘制Focus
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(m_iconAndTextSpace, m_iconAndTextSpace, -m_iconAndTextSpace, -m_iconAndTextSpace);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            fr.rect.adjust(0, 0, -style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, w), 0);
        }
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, w);
    }

    drawIconAndLabel(opt, p, w);
}


/**
 * @brief 重新计算sizehint，sizehint函数在第一次计算完成后会记录结果，如果文字字体或者内容不变是不需要进行重复计算
 *
 * 正常来讲，高度是不需要管，因为高度是有RibbonGroupLayout来控制，这个函数主要是为了提供宽度信息，宽度信息给到RibbonGroupLayout，
 * 然后对button进行布局，最终会触发resize函数，resize函数会再进行一次计算确定具体的尺寸
 *
 * 因此，resize函数确定的尺寸方式必须和recalcSizeHint保持一致，尤其是涉及字体设置
 *
 * 一般布局前调用sizehint，布局后会调用resizeevent，尤其字体信息会计算两遍，这里后续可以有更好的优化空间
 */
void RibbonButtonPrivate::recalcSizeHint(QStyleOptionToolButton &opt, QSize s)
{
    // QToolButton的sizeHint已经考虑了菜单箭头的位置
    // 从源码看，QToolButton的sizeHint是不会考虑换行的
    if (RibbonButton::LargeButton == m_buttonType) {
        // 计算最佳大小
        if (RibbonGroup *group = qobject_cast<RibbonGroup *>(q->parent())) {
            // 对于建立在RibbonGroup的基础上的大按钮，把高度设置为RibbonGroup计算的大按钮高度
            s.setHeight(group->largeHeight());
        }
        if (RibbonButton::Normal == m_largeButtonType ||
                (RibbonButton::Lite == m_largeButtonType && s_liteStyleEnableWordWrap)) {
            // 普通模式下才涉及到文字换行，或者lite模式下指定了文字换行
            if (s.width() > s.height() * QX_WIDTH_HEIGHT_RATIO) {
                // 说明是一个长方形按钮
                // 文本对齐方式
                // 如果宽度大于高度，就看看换行是否能满足
                QFontMetrics fm = q->fontMetrics();
                QRect textRange(0, 0, s.width(), s.height());

                //! 1 先获取最优的文字textRange
                {
                    const int maxTrycount = 3;
                    int trycount = 0;
                    int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;
                    do {
                        // 先计算两行文本的紧凑矩形，从一半开始逐渐递增
                        textRange.setWidth(s.width() / 2 + (s.width() / 2) * (float(trycount) / maxTrycount));
                        textRange = fm.boundingRect(textRange, alignment, opt.text);
                        // Linux GNOME平台上，fm.lineSpacing()为15，textRange.height()为35
                        // Windows 10平台上，fm.lineSpacing()为16，textRange.height()为32
#ifdef Q_OS_WINDOWS
                        if (textRange.height() <= (fm.lineSpacing() * 2)) {
#else
                        if (textRange.height() <= (fm.lineSpacing() * 2.5)) {
#endif
                            // 保证在两行
                            m_isWordWrap = (textRange.height() > fm.lineSpacing());
                            break;
                        }
                        ++trycount;
                    } while (trycount < maxTrycount);
                }
                // 左右留2像素
                s.setWidth(textRange.width() + m_iconAndTextSpace * 2);
                //! 2再判断箭头来调整textRange
                // 把区域设置给size
                // 确认是否换行
                if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                    (opt.features & QStyleOptionToolButton::HasMenu)) {
                    // 如果有菜单
                    if (m_largeButtonType == RibbonButton::Lite) {
                        // lite模式下都要偏移
                        s.rwidth() += QX_INDICATOR_ARROW_WIDTH;
                    } else {
                        // Normal模式下，仅对换行的偏移
                        if (m_isWordWrap) {
                            s.rwidth() += QX_INDICATOR_ARROW_WIDTH;
                        }
                    }
                }
            } else {
                m_isWordWrap = opt.text.contains('\n');
                if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                    (opt.features & QStyleOptionToolButton::HasMenu)) {
                    // 如果有菜单
                    if (m_isWordWrap) {
                        s.rwidth() += QX_INDICATOR_ARROW_WIDTH;
                    }
                }
            }
            // FIXME：当设置 Lite 可换行时，width 加 1，保证可以触发 resizeEvent，根因尚不明
            if (RibbonButton::Lite == m_largeButtonType && s_liteStyleEnableWordWrap) {
                s.rwidth() += 1;
            }
        } else {
            // 否则就是lite模式，只允许1行，有菜单就偏移
            if (s.width() > s.height() * 2) {
                // 过于宽的按钮，把文字用...来替代
                s.rwidth() = s.height() * 2;
            }
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                (opt.features & QStyleOptionToolButton::HasMenu)) {
                // 如果有菜单
                s.rwidth() += QX_INDICATOR_ARROW_WIDTH;
            }
        }
        // 无需在这里进行计算，在resizeevent里进行计算
        //! 3在这时候能确定m_textRect,m_iconRect
        // m_textRect = textRange.moveBottomLeft(QPoint(s.width() - m_iconAndTextSpace, s.height() -
        //     m_iconAndTextSpace));
        // that->calcIconRect(QRect(0, 0, s.width(), s.height()), m_textRect, opt);
    } else {
        // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
        // 在仅有图标的小模式显示时，预留一个下拉箭头位置
        if (Qt::ToolButtonIconOnly == opt.toolButtonStyle) {
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
                opt.features & QStyleOptionToolButton::HasMenu) {
                // 如果有菜单
                // s.rwidth() += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
                s.rwidth() += QX_INDICATOR_ARROW_WIDTH;
            }
        }
    }
    m_sizeHint = s;
}

/**
 * @brief RibbonButton::drawIconAndLabel
 *
 * 在LargeButtonType == Normal模式下，icon占大按钮的一半区域，在wps模式下，icon占大按钮的60%，文字占40%，且文字不换行
 * @param p
 * @param opt
 */
void RibbonButtonPrivate::drawIconAndLabel(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w)
{
    if (RibbonButton::LargeButton == m_buttonType) {
        // 绘制图标和文字
        QRect textRect = adjustedTextRect(opt, w);
        bool isArrow = opt.features & QStyleOptionToolButton::Arrow;
        if (((!isArrow && opt.icon.isNull()) && !opt.text.isEmpty()) ||
            (opt.toolButtonStyle == Qt::ToolButtonTextOnly)) {
            // 没有箭头 且 没图标 有文字 || 只有文字模式
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextWordWrap;   // 纯文本下，居中对齐,换行
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                alignment |= Qt::TextHideMnemonic;
            }
            p.setFont(opt.font);
            style()->drawItemText(&p, textRect, alignment, opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                                  QPalette::ButtonText);
        } else {
            // 非纯文本
            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                // 文本加图标情况
                p.setFont(opt.font);
                int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;   // 换行
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                    alignment |= Qt::TextHideMnemonic;
                }
                // 文字在icon下
                // 先绘制图标
                if (!isArrow) {
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
                    HELP_DRAW_RECT(p, m_iconRect);
#endif
                    QPixmap pm = createIconPixmap(opt, m_iconRect.size());
                    style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
                } else {
                    drawArrow(style(), &opt, m_iconRect, &p, w);
                }
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
                HELP_DRAW_RECT(p, textRect);
#endif
                if (RibbonButton::Lite == m_largeButtonType && !s_liteStyleEnableWordWrap) {
                    alignment |= Qt::AlignCenter;
                } else {
                    alignment |= Qt::AlignHCenter | Qt::AlignTop;   // 文字是顶部对齐
                }

                // 再绘制文本，对于Normal模式下的Largebutton，如果有菜单，且m_isWordWrap是true，箭头将在文本旁边
                if (RibbonButton::Lite == m_largeButtonType && !s_liteStyleEnableWordWrap) {
                    // lite 模式，文字不换行, 显示的内容需要进行省略处理
                    opt.text =
                        w->fontMetrics().elidedText(opt.text, Qt::ElideRight, textRect.width(), Qt::TextShowMnemonic);
                }
                style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, textRect), alignment, opt.palette,
                                      opt.state & QStyle::State_Enabled, opt.text, QPalette::ButtonText);
            } else {
                // 只有图标情况
                if (isArrow) {
                    drawArrow(style(), &opt, opt.rect, &p, w);
                } else {
                    QPixmap pm = createIconPixmap(opt, m_iconRect.size());
                    style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
                }
            }
            // 绘制sub control 的下拉箭头
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
                opt.features & QStyleOptionToolButton::HasMenu) {
                opt.rect = calcIndicatorArrowDownRect(opt);
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
                HELP_DRAW_RECT(p, opt.rect);
#endif
                style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &p, w);
            }
        }
    } else {
        // 小图标
        if (!opt.icon.isNull()) {
            QPixmap pm = createIconPixmap(opt, m_iconRect.size());

            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                p.save();
                p.setFont(opt.font);

                QRect pr = m_iconRect;                                    // 图标区域
                QRect tr = opt.rect.adjusted(pr.width() + 2, 0, -1, 0);   // 文本区域
                int alignment = Qt::TextShowMnemonic;
                // 快捷键的下划线
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                    alignment |= Qt::TextHideMnemonic;
                }
                if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                    // ribbonbutton在小图标下，不支持ToolButtonTextUnderIcon
                } else {
                    style()->drawItemPixmap(&p, QStyle::visualRect(opt.direction, opt.rect, pr), Qt::AlignCenter, pm);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, tr), alignment, opt.palette,
                                      opt.state & QStyle::State_Enabled, opt.text, QPalette::ButtonText);
                p.restore();
            } else {
                style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
            }
        } else {   // 只有文字
            int alignment = Qt::TextShowMnemonic;
            // 快捷键的下划线
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                alignment |= Qt::TextHideMnemonic;
            }
            style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, opt.rect.adjusted(2, 1, -2, -1)),
                                  alignment, opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                                  QPalette::ButtonText);
        }

        // 绘制sub control 的下拉箭头
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
            QStyleOptionToolButton tool = opt;
            tool.rect = calcIndicatorArrowDownRect(tool);
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            HELP_DRAW_RECT(p, tool.rect);
#endif
            style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, w);
        }
    }
}

void RibbonButtonPrivate::drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton, const QRect &rect,
                                  QPainter *painter, const QWidget *widget)
{
    QStyle::PrimitiveElement pe;

    switch (toolbutton->arrowType) {
    case Qt::LeftArrow:
        pe = QStyle::PE_IndicatorArrowLeft;
        break;
    case Qt::RightArrow:
        pe = QStyle::PE_IndicatorArrowRight;
        break;
    case Qt::UpArrow:
        pe = QStyle::PE_IndicatorArrowUp;
        break;
    case Qt::DownArrow:
        pe = QStyle::PE_IndicatorArrowDown;
        break;
    default:
        return;
    }

    QStyleOption arrowOpt = *toolbutton;
    arrowOpt.rect = rect;
    style->drawPrimitive(pe, &arrowOpt, painter, widget);
}

/**
 * @brief 根据设定计算图标和文本的绘制区域
 * @param opt
 * @return
 */
void RibbonButtonPrivate::calcIconAndTextRect(const QStyleOptionToolButton &opt)
{
    if (RibbonButton::LargeButton == m_buttonType) {
        m_iconRect =
            opt.rect.adjusted(m_iconAndTextSpace, m_iconAndTextSpace, -m_iconAndTextSpace, -m_iconAndTextSpace);
        if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
            // sizehint已经保证宽度能显示两行
            // icon 的区域就是文字以外的区域，如果m_isEnableWordWrap=true，就保证有两行文本的空间
            QFontMetrics fm = q->fontMetrics();
            int maxheight = 30;
            if (RibbonButton::Normal == m_largeButtonType ||
                    (RibbonButton::Lite == m_largeButtonType && s_liteStyleEnableWordWrap)) {
                maxheight = opt.rect.height() - (fm.lineSpacing() * 2)   // 减去2行文本
                            - 2 * m_iconAndTextSpace                     // 减去赏析边距
                            - m_iconAndTextSpace; // 这里减去m_iconAndTextSpace，m_iconAndTextSpace是icon和text的分隔距离
            } else {
                maxheight = opt.rect.height() - (fm.lineSpacing() * 1.2) // 减去1行文本
                            - 2 * m_iconAndTextSpace                     // 减去赏析边距
                            - m_iconAndTextSpace; // 这里减去m_iconAndTextSpace，m_iconAndTextSpace是icon和text的分隔距离
            }
            m_iconRect.setHeight(maxheight);

            // 没有菜单
            m_textRect.setRect(m_iconAndTextSpace, m_iconRect.bottom() + m_iconAndTextSpace, m_iconRect.width(),
                               opt.rect.bottom() - m_iconRect.bottom() -
                               m_iconAndTextSpace   // 高度是矩形底部减去icon底部再减去2*Space
            );
            // 保证能显示两行文本
            // 下面确定文字区域
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                (opt.features & QStyleOptionToolButton::HasMenu)) {
                // 有菜单且换行,宽度偏移ARROW_WIDTH
                if (m_isWordWrap) {
                    m_textRect.adjust(0, 0, -QX_INDICATOR_ARROW_WIDTH, 0);
                } else if (RibbonButton::Lite == m_largeButtonType && !s_liteStyleEnableWordWrap) {
                    // 在lite模式下，不允许换行的时候，也需要偏移下三角
                    m_textRect.adjust(0, 0, -QX_INDICATOR_ARROW_WIDTH, 0);
                }
            }
        } else {
            m_textRect = QRect();
        }
    } else {
        // 小按钮
        if (opt.toolButtonStyle == Qt::ToolButtonIconOnly) {
            // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            m_iconRect =
                opt.rect.adjusted(m_iconAndTextSpace, m_iconAndTextSpace, -m_iconAndTextSpace, -m_iconAndTextSpace);
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
                opt.features & QStyleOptionToolButton::HasMenu) {
                m_iconRect.adjust(0, 0, -QX_INDICATOR_ARROW_WIDTH, 0);
            }
            // 确定文本区域
            m_textRect = QRect();
        } else {
            m_iconRect = QRect(m_iconAndTextSpace, m_iconAndTextSpace,
                               qMax(opt.rect.height(), opt.iconSize.width()) - 2 * m_iconAndTextSpace,
                               opt.rect.height() - 2 * m_iconAndTextSpace);
            m_textRect.setRect(m_iconRect.right() + m_iconAndTextSpace, m_iconAndTextSpace,
                               opt.rect.right() - m_iconRect.right() - 2 * m_iconAndTextSpace,
                               opt.rect.height() - 2 * m_iconAndTextSpace);
            // 确定文本区域
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) ||
                (opt.features & QStyleOptionToolButton::HasMenu)) {
                // 有菜单,宽度偏移ARROW_WIDTH
                m_textRect.adjust(0, 0, -QX_INDICATOR_ARROW_WIDTH, 0);
            }
        }
    }

    // 纯文本的文字位置
    bool isArrow = opt.features & QStyleOptionToolButton::Arrow;
    if ((Qt::ToolButtonTextOnly == opt.toolButtonStyle) || (opt.icon.isNull() && !isArrow)) {
        m_textRect = opt.rect.adjusted(m_iconAndTextSpace, m_iconAndTextSpace,
                                        -m_iconAndTextSpace, -m_iconAndTextSpace);
    }
}

/**
 * @brief 根据设定计算文本显示区域
 * @param opt
 * @return
 */
QRect RibbonButtonPrivate::adjustedTextRect(const QStyleOptionToolButton &opt, const QWidget *w)
{
    QRect rect = m_textRect;

    // 这两个偏移量是有些按钮在点击时文字会出现一个类似抖动的效果，但据观察office2013的ribbon是不会动的，因此把这个效果取消
    if (s_isToolButtonTextShift) {
        int shiftX = 0;
        int shiftY = 0;

        if (opt.state & (QStyle::State_Sunken | QStyle::State_On)) {
            shiftX = style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, w);
            shiftY = style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, w);
        }

        rect.translate(shiftX, shiftY);
    }
    return rect;
}

/**
 * @brief sub control 的下拉箭头的位置
 *
 * 对于大模式，如果文字换行，箭头会在最右侧，如果文字不换行，箭头就在原来文字第二行的位置
 * @param opt
 * @return
 * @note IndicatorArrow尺寸的计算依赖text，前置需要先调用calcIconAndTextRect
 */
QRect RibbonButtonPrivate::calcIndicatorArrowDownRect(const QStyleOptionToolButton &opt)
{
    // 预留 QX_INDICATOR_ARROW_WIDTH px绘制箭头，1px的上下边界
    QRect rect = opt.rect;
    if (RibbonButton::LargeButton == m_buttonType) {
        // Lite不换行和换行两种情况下，箭头的大小通过rect的宽度控制（见calcIconAndTextRect中的m_textRect），
        // 否则通过rect的高度控制（见下文的QX_INDICATOR_ARROW_WIDTH）
        if ((RibbonButton::Lite == m_largeButtonType && !s_liteStyleEnableWordWrap)) {
            // 首先判断是否为lite且不允许换行
            // yh + QX_INDICATOR_ARROW_WIDTH + yh = m_textRect.height()
            // yh = (m_textRect.height() - QX_INDICATOR_ARROW_WIDTH) / 2
            // y = m_textRect.top() + yh
            int y = m_textRect.top() + (m_textRect.height() - QX_INDICATOR_ARROW_WIDTH) / 2;
            rect.setRect(m_textRect.right(), y,
                         opt.rect.right() - m_iconAndTextSpace - m_textRect.right(), QX_INDICATOR_ARROW_WIDTH);
        } else if (m_isWordWrap) {
            // 如果允许换行，那么就在文本第二行最后端，菜单的下拉箭头位于第二行文本的矩形区域
            rect.setRect(m_textRect.right(), m_textRect.top() + m_textRect.height() / 2,
                         opt.rect.right() - m_iconAndTextSpace - m_textRect.right(), m_textRect.height() / 2);
        } else {
            // 都不是的情况下就第二行文本位置       |y坐标
            // m_textRect.height() / 2 + yh + QX_INDICATOR_ARROW_WIDTH + yh = m_textRect.height()
            // yh = m_textRect.height() / 4 - QX_INDICATOR_ARROW_WIDTH / 2
            // y = m_textRect.bottom() - yh - QX_INDICATOR_ARROW_WIDTH
            int y = m_textRect.bottom() - m_textRect.height() / 4 - QX_INDICATOR_ARROW_WIDTH / 2;
            rect.setRect(m_textRect.left(), y, m_textRect.width(), QX_INDICATOR_ARROW_WIDTH);
        }
    } else {
        rect.setRect(rect.width() - QX_INDICATOR_ARROW_WIDTH - m_iconAndTextSpace, m_iconAndTextSpace,
                     QX_INDICATOR_ARROW_WIDTH, rect.height() - 2 * m_iconAndTextSpace);
    }
    return rect;
}

QPixmap RibbonButtonPrivate::createIconPixmap(const QStyleOptionToolButton &opt, const QSize &iconsize)
{
    if (!opt.icon.isNull()) {   // 有图标
        QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
        QIcon::Mode mode;
        if (!(opt.state & QStyle::State_Enabled)) {
            mode = QIcon::Disabled;
        } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
            mode = QIcon::Active;
        } else {
            mode = QIcon::Normal;
        }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        // return (opt.icon.pixmap(q->window()->windowHandle(), opt.rect.size().boundedTo(realConSize), mode, state));
        return opt.icon.pixmap(q->window()->windowHandle(), iconsize, mode, state);
#else
        return opt.icon.pixmap(iconsize, q->window()->devicePixelRatio(), mode, state);
#endif
    }
    return QPixmap();
}

void RibbonButtonPrivate::drawButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w)
{
    switch (m_buttonType) {
    case RibbonButton::LargeButton:
        drawLargeButton(opt, p, w);
        break;
    case RibbonButton::SmallButton:
        drawSmallButton(opt, p, w);
        break;
    default:
        break;
    }
}

RibbonButton::RibbonButton(QWidget *parent)
    : QToolButton(parent)
    , d(new RibbonButtonPrivate())
{
    d->q = this;
    setAutoRaise(true);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

RibbonButton::RibbonButton(QAction *defaultAction, QWidget *parent)
    : RibbonButton(parent)
{
    setDefaultAction(defaultAction);
}

RibbonButton::~RibbonButton()
{
    delete d;
}

void RibbonButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d->drawButton(opt, p, this);
}

void RibbonButton::mouseMoveEvent(QMouseEvent *e)
{
    bool isMouseOnSubControl(false);

    if (d->m_iconRect.isValid()) {
        isMouseOnSubControl = !d->m_iconRect.contains(e->pos());
    }

    if (d->m_mouseOnSubControl != isMouseOnSubControl) {
        d->m_mouseOnSubControl = isMouseOnSubControl;
        // 从icon变到text过程中刷新一次
        update();
    }
    QToolButton::mouseMoveEvent(e);
}

void RibbonButton::mousePressEvent(QMouseEvent *e)
{
    if ((e->button() == Qt::LeftButton) && (popupMode() == MenuButtonPopup)) {
        if (LargeButton == d->m_buttonType) {
            QRect popupr = rect().adjusted(0, height() / 2, 0, 0);
            if (popupr.isValid() && popupr.contains(e->pos())) {
                d->m_menuButtonPressed = true;
                showMenu();
                return;
            }
        } else {
            if (d->m_iconRect.isValid() && !d->m_iconRect.contains(e->pos())) {
                d->m_menuButtonPressed = true;
                showMenu();
                return;
            }
        }
    }
    d->m_menuButtonPressed = false;
    QToolButton::mousePressEvent(e);
}

void RibbonButton::mouseReleaseEvent(QMouseEvent *e)
{
    QToolButton::mouseReleaseEvent(e);
    d->m_menuButtonPressed = false;
}

void RibbonButton::focusOutEvent(QFocusEvent *e)
{
    QToolButton::focusOutEvent(e);
    d->m_mouseOnSubControl = false;
}

void RibbonButton::leaveEvent(QEvent *e)
{
    d->m_mouseOnSubControl = false;
    QToolButton::leaveEvent(e);
}

bool RibbonButton::hitButton(const QPoint &pos) const
{
    if (QAbstractButton::hitButton(pos)) {
        return !d->m_menuButtonPressed;
    }
    return false;
}

/**
 * @brief 在 resizeevent 计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
 * @param e
 */
void RibbonButton::resizeEvent(QResizeEvent *e)
{
#if DebugRibbonButton_TextDrawPrint
    qDebug() << "RibbonButton::resizeEvent, text=" << text() << " obj=" << objectName() << " size=" << e->size();
#endif
    // 在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d->calcIconAndTextRect(opt);
}

/**
 * @brief toolbutton 的尺寸确定是先定下字体的尺寸，再定下icon的尺寸，自底向上，保证字体能显示两行
 * @note m_sizeHint 的刷新需要注意
 * @return
 */
QSize RibbonButton::sizeHint() const
{
    // if (d->m_sizeHint.isValid()) {
    //     return d->m_sizeHint;
    // }
    QSize sz = QToolButton::sizeHint();
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d->recalcSizeHint(opt, sz);
    return d->m_sizeHint;
}

void RibbonButton::changeEvent(QEvent *e)
{
    if (e) {
        if (e->type() == QEvent::FontChange) {
            // todo 说明字体改变，需要重新计算和字体相关的信息
            QStyleOptionToolButton opt;
            initStyleOption(&opt);
            d->calcIconAndTextRect(opt);
        }
    }
    QToolButton::changeEvent(e);
}

void RibbonButton::actionEvent(QActionEvent *e)
{
    QToolButton::actionEvent(e);
    // action的文字发生变更时要重新计算尺寸--sizehint实时计算情况下，此处不需要这样设置
    // if (e) {
    //     // action发生变化
    //     if (e->action()) {
    //         if (!e->action()->text().isEmpty()) {
    //             QSize sz = QToolButton::sizeHint();
    //             QStyleOptionToolButton opt;
    //             initStyleOption(&opt);
    //             d->recalcSizeHint(opt, sz);
    //         }
    //     }
    // }
}

RibbonButton::RibbonButtonType RibbonButton::buttonType() const
{
    return d->m_buttonType;
}

/**
 * @brief 设置按钮样式
 * @note 设置按钮样式过程会调用 setToolButtonStyle, 如果要改变 toolButtonStyle,
 * 如设置为 Qt::ToolButtonIconOnly, 需要在此函数之后设置
 * @param type
 */
void RibbonButton::setButtonType(RibbonButton::RibbonButtonType type)
{
    d->m_buttonType = type;
    if (LargeButton == type) {
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    } else {
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    }
}

QSize RibbonButton::minimumSizeHint() const
{
    return sizeHint();
}

void RibbonButton::setLargeButtonType(RibbonButton::LargeButtonType type)
{
    d->m_largeButtonType = type;
}

RibbonButton::LargeButtonType RibbonButton::largeButtonType() const
{
    return d->m_largeButtonType;
}

/**
 * @brief 设置按钮点击时文字会闪烁一下
 * @param on
 */
void RibbonButton::setToolButtonTextShift(bool on)
{
    RibbonButtonPrivate::s_isToolButtonTextShift = on;
}

bool RibbonButton::isToolButtonTextShift()
{
    return RibbonButtonPrivate::s_isToolButtonTextShift;
}

/**
 * @brief 设置在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
 * @param on
 * @note 此函数的调用最好在 ribbonbar 布局之前设置，且调用之后需要调用 @sa RibbonStyleOption::recalc 刷新
 * @code
 * RibbonElementStyleOpt.recalc();
 * @endcode
 */
void RibbonButton::setLiteStyleEnableWordWrap(bool on)
{
    RibbonButtonPrivate::s_liteStyleEnableWordWrap = on;
}

bool RibbonButton::isLiteStyleEnableWordWrap()
{
    return RibbonButtonPrivate::s_liteStyleEnableWordWrap;
}

bool RibbonButton::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::WindowDeactivate:
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
    case QEvent::ActionAdded:
        d->m_mouseOnSubControl = false;
        break;
    default:
        break;
    }

    return QToolButton::event(e);
}
