/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonStyleOption.h"
#include "QxRibbonGroupLayout.h"
#include <QApplication>
#include <QDebug>

class RibbonStyleOptionPrivate
{
public:
    RibbonStyleOptionPrivate();
public:
    int m_tabBarHeight;
    int m_titleBarHeight;
    int m_ribbonBarHeightOfficeStyleThreeRow;
    int m_ribbonBarHeightWpsLiteStyleThreeRow;
    int m_ribbonBarHeightOfficeStyleTwoRow;
    int m_ribbonBarHeightWpsLiteStyleTwoRow;
};

RibbonStyleOptionPrivate::RibbonStyleOptionPrivate()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int lineSpacing = QApplication::fontMetrics().lineSpacing();
#else
    int lineSpacing = QFontMetricsF(QApplication::font()).lineSpacing();
#endif

    m_titleBarHeight = lineSpacing * 1.8;
    m_tabBarHeight = lineSpacing * 1.5;
}

RibbonStyleOption::RibbonStyleOption()
    : d(new RibbonStyleOptionPrivate)
{
    recalc();
}

RibbonStyleOption::~RibbonStyleOption()
{
    delete d;
}

int RibbonStyleOption::ribbonBarHeight(RibbonBar::RibbonStyle s) const
{
    switch (s) {
    case RibbonBar::OfficeStyle:
        return d->m_ribbonBarHeightOfficeStyleThreeRow;
    case RibbonBar::WpsLiteStyle:
        return d->m_ribbonBarHeightWpsLiteStyleThreeRow;
    case RibbonBar::OfficeStyleTwoRow:
        return d->m_ribbonBarHeightOfficeStyleTwoRow;
    case RibbonBar::WpsLiteStyleTwoRow:
        return d->m_ribbonBarHeightWpsLiteStyleTwoRow;
    default:
        break;
    }
    return d->m_ribbonBarHeightOfficeStyleThreeRow;
}

int RibbonStyleOption::titleBarHeight() const
{
    return d->m_titleBarHeight;
}

int RibbonStyleOption::tabBarHeight() const
{
    return d->m_tabBarHeight;
}

void RibbonStyleOption::recalc()
{
    d->m_ribbonBarHeightOfficeStyleThreeRow = calcRibbonBarHeight(RibbonBar::OfficeStyle);
    d->m_ribbonBarHeightWpsLiteStyleThreeRow = calcRibbonBarHeight(RibbonBar::WpsLiteStyle);
    d->m_ribbonBarHeightOfficeStyleTwoRow = calcRibbonBarHeight(RibbonBar::OfficeStyleTwoRow);
    d->m_ribbonBarHeightWpsLiteStyleTwoRow = calcRibbonBarHeight(RibbonBar::WpsLiteStyleTwoRow);
}

int RibbonStyleOption::calcRibbonBarHeight(RibbonBar::RibbonStyle s) const
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int lineSpacing = QApplication::fontMetrics().lineSpacing();
#else
    int lineSpacing = QFontMetricsF(QApplication::font()).lineSpacing();
#endif
    switch (s) {
    case RibbonBar::OfficeStyle:
        return titleBarHeight() + tabBarHeight() + (lineSpacing * 1.5) * 3 +
               RibbonGroupLayout::groupContentsMargins().top() + RibbonGroupLayout::groupContentsMargins().bottom() +
               RibbonGroup::groupTitleHeight();
    case RibbonBar::WpsLiteStyle:
        return d->m_ribbonBarHeightOfficeStyleThreeRow - tabBarHeight();
    case RibbonBar::WpsLiteStyleTwoRow:
        return d->m_ribbonBarHeightOfficeStyleThreeRow * 0.95 - tabBarHeight() - RibbonGroup::groupTitleHeight();
    case RibbonBar::OfficeStyleTwoRow:
        return d->m_ribbonBarHeightOfficeStyleThreeRow * 0.95 - RibbonGroup::groupTitleHeight();
    default:
        break;
    }
    return d->m_ribbonBarHeightOfficeStyleThreeRow;
}

QDebug operator<<(QDebug debug, const RibbonStyleOption &c)
{
    QDebugStateSaver saver(debug);
    Q_UNUSED(saver);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int lineSpacing = QApplication::fontMetrics().lineSpacing();
#else
    int lineSpacing = QFontMetricsF(QApplication::font()).lineSpacing();
#endif
    debug.nospace() << "fontMetrics.lineSpacing=" << lineSpacing
                    << ",RibbonStyleOption(titleBarHeight=" << c.titleBarHeight()
                    << ",tabBarHeight=" << c.tabBarHeight()
                    << "\n,ribbonBarHeight(OfficeStyle)=" << c.ribbonBarHeight(RibbonBar::OfficeStyle)
                    << "\n,ribbonBarHeight(OfficeStyleTwoRow)=" << c.ribbonBarHeight(RibbonBar::OfficeStyleTwoRow)
                    << "\n,ribbonBarHeight(WpsLiteStyle)=" << c.ribbonBarHeight(RibbonBar::WpsLiteStyle)
                    << "\n,ribbonBarHeight(WpsLiteStyleTwoRow)=" << c.ribbonBarHeight(RibbonBar::WpsLiteStyleTwoRow);
    return debug;
}

