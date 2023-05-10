/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "QxRibbonPageContext.h"
#include "QxRibbonManager.h"

#include <QDebug>
#include <QList>
#include <QVariant>


/* RibbonPageData */
class RibbonPageData
{
public:
    RibbonPage *page;
};

/* RibbonPageContextPrivate */
class RibbonPageContextPrivate
{
public:
    QList<RibbonPageData> pageDataList;
    QVariant contextID;
    QColor contextColor;
    QString contextTitle;
};

RibbonPageContext::RibbonPageContext(QWidget *parent)
    : QObject(parent), d(new RibbonPageContextPrivate)
{
}

RibbonPageContext::~RibbonPageContext()
{
    delete d;
}

RibbonPage *RibbonPageContext::addPage(const QString &title)
{
    RibbonPageData pageData;
    RibbonPage *page = new RibbonPage(parentWidget());

    page->markIsPageContext(true);
    page->setWindowTitle(title);
    pageData.page = page;
    d->pageDataList.append(pageData);
    page->installEventFilter(this);
    emit pageAdded(page);

    return page;
}

int RibbonPageContext::pageCount() const
{
    return d->pageDataList.count();
}

void RibbonPageContext::setId(const QVariant &id)
{
    d->contextID = id;
}

QVariant RibbonPageContext::id() const
{
    return d->contextID;
}

void RibbonPageContext::setContextColor(const QColor color)
{
    d->contextColor = color;
}

QColor RibbonPageContext::contextColor() const
{
    return d->contextColor;
}

QWidget *RibbonPageContext::parentWidget() const
{
    return qobject_cast<QWidget *>(parent());
}

bool RibbonPageContext::eventFilter(QObject *watched, QEvent *e)
{
    if (Q_NULLPTR == watched) {
        return false;
    }
    switch (e->type()) {
    case QEvent::Close: {
        RibbonPage *c = qobject_cast<RibbonPage *>(watched);
        if (c) {
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            qDebug() << " -----------> close event";
#endif
            takePage(c);
        }
    } break;

    default:
        break;
    }
    return false;
}

/**
 * @brief 获取上下文标签的标题
 * @return
 */
QString RibbonPageContext::contextTitle() const
{
    return d->contextTitle;
}

/**
 * @brief 设置上下文标签的标题，标题仅在 office 模式下显示 @ref RibbonBar::RibbonStyle
 * @param title
 */
void RibbonPageContext::setContextTitle(const QString &title)
{
    d->contextTitle = title;
}

/**
 * @brief 获取对应的tab页
 * @param index
 * @return
 */
RibbonPage *RibbonPageContext::page(int index)
{
    if (index < 0 || index >= d->pageDataList.size()) {
        return Q_NULLPTR;
    }
    return d->pageDataList[index].page;
}

/**
 * @brief 获取所有的RibbonPage*
 * @return
 */
QList<RibbonPage *> RibbonPageContext::pageList() const
{
    QList<RibbonPage *> res;

    for (RibbonPageData &c : d->pageDataList) {
        res.append(c.page);
    }
    return res;
}

/**
 * @brief 移除这个page，这时RibbonPageContext不再管理这个page
 * @param page
 * @return 成功移除返回true
 */
bool RibbonPageContext::takePage(RibbonPage *page)
{
    for (int i = 0; i < d->pageDataList.size(); ++i) {
        if (d->pageDataList[i].page == page) {
            d->pageDataList.takeAt(i);
            return true;
        }
    }
    return false;
}
