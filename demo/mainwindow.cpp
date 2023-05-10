#include "mainwindow.h"

#include "QxRibbonApplicationButton.h"
#include "QxRibbonBar.h"
#include "QxRibbonButton.h"
#include "QxRibbonButtonGroup.h"
#include "QxRibbonControls.h"
#include "QxRibbonCustomizeDialog.h"
#include "QxRibbonCustomizeWidget.h"
#include "QxRibbonManager.h"
#include "QxRibbonGallery.h"
#include "QxRibbonGroup.h"
#include "QxRibbonPage.h"
#include "QxRibbonQuickAccessBar.h"

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCalendarWidget>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextStream>
#include <QXmlStreamWriter>

QX_USE_NAMESPACE

#define PRINT_COST_START()                                                                                             \
    QElapsedTimer __TMP_COST;                                                                                          \
    __TMP_COST.start();                                                                                                \
    int __TMP_LASTTIMES = 0

#define PRINT_COST(STR)                                                                                                \
    do {                                                                                                               \
        int ___TMP_INT = __TMP_COST.elapsed();                                                                         \
        qDebug() << STR << " cost " << ___TMP_INT - __TMP_LASTTIMES << " ms (" << ___TMP_INT << ")";                   \
        m_edit->append(QString("%1 cost %2 ms(%3)").arg(STR).arg(___TMP_INT - __TMP_LASTTIMES).arg(___TMP_INT));       \
        __TMP_LASTTIMES = ___TMP_INT;                                                                                  \
    } while (0)

MainWindow::MainWindow(QWidget *par)
    : RibbonWindow(par)
    , m_customizeWidget(nullptr)
{
    PRINT_COST_START();
    setWindowTitle(("ribbon mainwindow demo"));
    m_edit = new QTextEdit(this);
    setCentralWidget(m_edit);
    setStatusBar(new QStatusBar());

    RibbonBar *ribbon = ribbonBar();
    // 通过setContentsMargins设置ribbon四周的间距
    ribbon->setContentsMargins(5, 0, 5, 0);
    // 设置applicationButton
    PRINT_COST("setCentralWidget & setWindowTitle");
    ribbon->applicationButton()->setText(("File"));

    // 添加主标签页 - 通过addPage工厂函数添加
    RibbonPage *pageMain = ribbon->addPage(tr("Main"));
    pageMain->setObjectName(("pageMain"));
    createPageMain(pageMain);
    PRINT_COST("new main page");

    // 添加其他标签页 - 直接new RibbonPage添加
    RibbonPage *pageOther = new RibbonPage();
    pageOther->setPageName(tr("Other"));
    pageOther->setObjectName(("pageOther"));
    createPageOther(pageOther);
    ribbon->addPage(pageOther);
    PRINT_COST("add other page");

    // 添加删除标签页
    RibbonPage *pageDelete = new RibbonPage();

    pageDelete->setPageName(("Delete"));
    pageDelete->setObjectName(("pageDelete"));
    ribbon->addPage(pageDelete);
    createPageDelete(pageDelete);
    PRINT_COST("add page delete page");
    // 添加尺寸标签页
    RibbonPage *pageSize = new RibbonPage();

    pageSize->setPageName(("Size(example long page)"));
    pageSize->setObjectName(("pageSize"));
    ribbon->addPage(pageSize);
    createPageSize(pageSize);
    PRINT_COST("add page delete page");
    createPageContext1();
    PRINT_COST("add context1 page page");
    createPageContext2();
    PRINT_COST("add context2 page page");
    RibbonQuickAccessBar *quickAccessBar = ribbon->quickAccessBar();
    createQuickAccessBar(quickAccessBar);
    PRINT_COST("add quick access bar");
    RibbonButtonGroup *rightBar = ribbon->rightButtonGroup();
    createRightButtonGroup(rightBar);
    PRINT_COST("add right bar");
    addSomeOtherAction();

    setMinimumWidth(500);
    showMaximized();
    setWindowIcon(QIcon(":/icon/res/logo.png"));

    qDebug() << RibbonSubElementStyleOpt;
}

void MainWindow::onShowContextPage(bool on)
{
    if (m_pageContext == nullptr) {
        createPageContext1();
    }
    if (on) {
        ribbonBar()->showPageContext(m_pageContext);
    } else {
        ribbonBar()->hidePageContext(m_pageContext);
    }
}

void MainWindow::onStyleClicked(int id)
{
    ribbonBar()->setRibbonStyle(static_cast<RibbonBar::RibbonStyle>(id));
}

void MainWindow::onActionCustomizeTriggered(bool b)
{
    Q_UNUSED(b);
    if (nullptr == m_customizeWidget) {
        m_customizeWidget =
            new RibbonCustomizeWidget(this, this, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::Dialog);
        m_customizeWidget->setWindowModality(Qt::ApplicationModal);   // 设置阻塞类型
        m_customizeWidget->setAttribute(Qt::WA_ShowModal, true);      // 属性设置 true:模态 false:非模态
        m_customizeWidget->setupActionsManager(m_actMgr);
    }
    m_customizeWidget->show();
    m_customizeWidget->applys();
}

void MainWindow::onActionCustomizeAndSaveTriggered(bool b)
{
    Q_UNUSED(b);
    RibbonCustomizeDialog dlg(this);
    dlg.setupActionsManager(m_actMgr);
    dlg.fromXml("customize.xml");
    if (RibbonCustomizeDialog::Accepted == dlg.exec()) {
        dlg.applys();
        QByteArray str;
        QXmlStreamWriter xml(&str);
        xml.setAutoFormatting(true);
        xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)   // QXmlStreamWriter always encodes XML in UTF-8.
        xml.setCodec("utf-8");
#endif
        xml.writeStartDocument();
        bool isok = dlg.toXml(&xml);
        xml.writeEndDocument();
        if (isok) {
            QFile f("customize.xml");
            if (f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream s(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)   // QTextStream always encodes XML in UTF-8.
                s.setCodec("utf-8");
#endif
                s << str;
                s.flush();
            }
            m_edit->append("write xml:");
            m_edit->append(str);
        }
    }
}

void MainWindow::onActionHelpTriggered()
{
    QMessageBox::information(this, tr("infomation"),
                             tr("\n ==============="
                                "\n RibbonBar version: %1"
                                "\n https://gitee.com/icanpool/QxRibbon"
                                "\n ===============")
                                 .arg(getRibbonVersion()));
}

void MainWindow::onActionRemoveAppBtnTriggered(bool b)
{
    if (b) {
        ribbonBar()->setApplicationButton(nullptr);
    } else {
        RibbonApplicationButton *actionRemoveAppBtn = new RibbonApplicationButton();
        actionRemoveAppBtn->setText(tr("File"));
        ribbonBar()->setApplicationButton(actionRemoveAppBtn);
    }
}

void MainWindow::onActionUseQssTriggered()
{
    QFile f("ribbon.qss");
    if (!f.exists()) {
        QString fdir = QFileDialog::getOpenFileName(this, tr("select qss file"));
        if (fdir.isEmpty()) {
            return;
        }
        f.setFileName(fdir);
    }
    if (!f.open(QIODevice::ReadWrite)) {
        return;
    }
    QString qss(f.readAll());
    m_edit->setText(qss);
    this->ribbonBar()->setStyleSheet(qss);
}

void MainWindow::onActionLoadCustomizeXmlFileTriggered()
{
    // 只能调用一次
    static bool has_call = false;
    if (!has_call) {
        has_call = ribbon_apply_customize_from_xml_file("customize.xml", this, m_actMgr);
    }
}

void MainWindow::onActionWindowFlagNormalButtonTriggered(bool b)
{
    if (b) {
        // 最大最小关闭按钮都有
        Qt::WindowFlags f = windowFlags();
        f |= (Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        updateWindowFlag(f);
    } else {
        // 由于已经处于frameless状态，这个最大最小设置是无效的
        //  setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
        Qt::WindowFlags f = windowFlags();
        f &= ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowCloseButtonHint;
        updateWindowFlag(f);
    }
}

void MainWindow::onFontComWidgetCurrentFontChanged(const QFont &f)
{
    qDebug() << "set font:" << f;
    ribbonBar()->setFont(f);
    update();
}

void MainWindow::onActionFontLargerTriggered()
{
    QFont f = font();
    f.setPointSize(f.pointSize() + 1);
    ribbonBar()->setFont(f);
    update();
    qDebug() << "set font:" << f;
}

void MainWindow::onActionFontSmallerTriggered()
{
    QFont f = font();
    f.setPointSize(f.pointSize() - 1);
    ribbonBar()->setFont(f);
    update();
    qDebug() << "set font:" << f;
}

void MainWindow::onActionwordWrapIn2rowTriggered(bool b)
{
    RibbonButton::setLiteStyleEnableWordWrap(b);   // 设置是否允许2行模式下文字换行，换行的话图标会较小
    // 换行设定后需要重新计算样式尺寸
    RibbonSubElementStyleOpt.recalc();
    // 通过setRibbonStyle来让ribbonbar重绘
    // 由于关键尺寸变化了，需要重新布局
    ribbonBar()->updateRibbonGeometry();
}

/**
 * @brief 测试RibbonButtonGroup和标题对齐
 * @param act
 */
void MainWindow::onButtonGroupActionTriggered(QAction *act)
{
    QVariant v = act->property("align");
    if (v.isValid()) {
        Qt::Alignment al = static_cast<Qt::Alignment>(v.toInt());
        if (!ribbonBar()) {
            return;
        }
        ribbonBar()->setWindowTitleAligment(al);
        ribbonBar()->repaint();
    }
}

void MainWindow::createPageMain(RibbonPage *page)
{
    //! 1
    //! group 1 start
    //!

    // 使用addGroup函数来创建RibbonGroup，效果和new RibbonGroup再addGroup一样
    RibbonGroup *groupStyle = page->addGroup(("ribbon style"));

    QAction *actSave = createAction(tr("Save"), ":/icon/res/save.svg");
    actSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    groupStyle->addLargeAction(actSave);
    connect(actSave, &QAction::triggered, this, [this](bool b) {
        Q_UNUSED(b);
        this->m_edit->append("actSaveion clicked");
    });

    QAction *actHideRibbon = createAction(tr("hide ribbon"), ":/icon/res/hideRibbon.svg", "actHideRibbon");
    actHideRibbon->setCheckable(true);
    groupStyle->addSmallAction(actHideRibbon);
    connect(actHideRibbon, &QAction::triggered, this, [this](bool b) {
        this->ribbonBar()->setMinimized(b);
    });
    connect(ribbonBar(), &RibbonBar::minimizationChanged, this, [actHideRibbon](bool minimized) {
        actHideRibbon->setChecked(minimized);
    });

    QAction *actShowHideButton =
        createAction(tr("show hide button"), ":/icon/res/showHideButton.svg", "show hide button");
    actShowHideButton->setCheckable(true);
    groupStyle->addSmallAction(actShowHideButton);
    connect(actShowHideButton, &QAction::triggered, this, [this](bool b) {
        this->ribbonBar()->showMinimumModeButton(b);   // 显示ribbon最小化按钮
    });
    actShowHideButton->trigger();

    QAction *actwordWrapIn2row = createAction(tr("word wrap in 2row"), ":/icon/res/wordwrap.svg");
    actwordWrapIn2row->setCheckable(true);
    groupStyle->addSmallAction(actwordWrapIn2row);
    connect(actwordWrapIn2row, &QAction::triggered, this, &MainWindow::onActionwordWrapIn2rowTriggered);

    QButtonGroup *g = new QButtonGroup(page);

    QRadioButton *r = new QRadioButton();
    r->setText(tr("use office style"));
    r->setObjectName(("use office style"));
    r->setChecked(true);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::OfficeStyle);

    r = new QRadioButton();
    r->setObjectName(("use wps style"));
    r->setText(tr("use wps style"));
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::WpsLiteStyle);

    r = new QRadioButton();
    r->setObjectName(("use office 2row style"));
    r->setText(tr("use office 2 row style"));
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::OfficeStyleTwoRow);

    r = new QRadioButton();
    r->setObjectName(("use wps 2row style"));
    r->setText(tr("use wps 2row style"));
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::WpsLiteStyleTwoRow);

//    connect(g, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MainWindow::onStyleClicked);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(g, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this, &MainWindow::onStyleClicked);
#else
    connect(g, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this,
            &MainWindow::onStyleClicked);
#endif

    RibbonGroup *groupToolButtonStyle = page->addGroup(("ribbon toolbutton style"));

    RibbonButton *btn;
    RibbonMenu *menu = new RibbonMenu(this);
    QAction *a = nullptr;
    {
        QIcon itemicon = QIcon(":/icon/res/item.svg");
        for (int i = 0; i < 5; ++i) {
            a = menu->addAction(itemicon, tr("item %1").arg(i + 1));
            a->setObjectName(QStringLiteral("item %1").arg(i + 1));
        }
    }

    QAction *act = createAction(tr("test 1"), ":/icon/res/test1.svg");
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::MenuButtonPopup mode"));
    btn = groupToolButtonStyle->addSmallAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    act = createAction(tr("test 2"), ":/icon/res/test2.svg");
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::InstantPopup mode"));
    btn = groupToolButtonStyle->addSmallAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);

    groupToolButtonStyle->addSeparator();

    act = createAction(tr("Delayed Popup"), ":/icon/res/folder-cog.svg");
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckabletriggered);

    act = createAction(tr("Menu Button Popup"), ":/icon/res/folder-star.svg");
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckabletriggered);

    act = createAction(tr("Instant Popup"), ":/icon/res/folder-stats.svg");
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onInstantPopupCheckabletriggered);

    act = createAction(tr("Delayed Popup checkable"), ":/icon/res/folder-table.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckableTest);

    act = createAction(tr("Menu Button Popup checkable"), ":/icon/res/folder-checkmark.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckableTest);

    act = createAction(tr("disable action"), ":/icon/res/disable.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    act->setEnabled(false);
    groupToolButtonStyle->addLargeAction(act);

    QAction *optAct = new QAction(this);
    connect(optAct, &QAction::triggered, this, [this](bool on) {
        Q_UNUSED(on);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    groupToolButtonStyle->addOptionAction(optAct);
    //! 2
    //! group 2 start
    //!
    RibbonGroup *group2 = page->addGroup(("group 2"));

    QAction *actShowContext = createAction(tr("show Context"), ":/icon/res/showContext.svg");
    actShowContext->setCheckable(true);
    group2->addLargeAction(actShowContext);
    connect(actShowContext, &QAction::triggered, this, &MainWindow::onShowContextPage);

    QAction *actDeleteContext = createAction(tr("delete Context"), ":/icon/res/deleteContext.svg");
    group2->addLargeAction(actDeleteContext);
    connect(actDeleteContext, &QAction::triggered, this, [this, act](bool on) {
        Q_UNUSED(on);
        if (this->m_pageContext) {
            this->ribbonBar()->destroyPageContext(this->m_pageContext);
            this->m_pageContext = nullptr;
            act->setDisabled(true);
        }
    });

    //! 3
    //! group 3 start -> widget test
    //!

    RibbonGroup *groupWidgetTest = page->addGroup(tr("widget test"));
    groupWidgetTest->setObjectName(QStringLiteral(u"groupWidgetTest"));

    RibbonComboBox *com = new RibbonComboBox(this);

    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle(tr("ComboBox"));
    for (int i = 0; i < 40; ++i) {
        com->addItem(QString("RibbonComboBox test%1").arg(i + 1));
    }
    com->setEditable(true);
    groupWidgetTest->addSmallWidget(com);

    com = new RibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle("ComboBox Editable");
    for (int i = 0; i < 40; ++i) {
        com->addItem(QString("item %1").arg(i + 1));
    }
    groupWidgetTest->addSmallWidget(com);

    RibbonLineEdit *lineEdit = new RibbonLineEdit(this);

    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineEdit->setWindowTitle("Line Edit");
    lineEdit->setText("RibbonLineEdit");
    groupWidgetTest->addSmallWidget(lineEdit);
    QWidget *w = lineEdit->parentWidget();

    while (w) {
        qDebug() << w->metaObject()->className();
        w = w->parentWidget();
    }
    RibbonCheckBox *checkBox = new RibbonCheckBox(this);

    checkBox->setText(tr("checkBox"));
    groupWidgetTest->addSmallWidget(checkBox);
    groupWidgetTest->addSeparator();

    QCalendarWidget *calendarWidget = new QCalendarWidget(this);
    calendarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    calendarWidget->setObjectName(("calendarWidget"));
    groupWidgetTest->addLargeWidget(calendarWidget);
    groupWidgetTest->setExpanding();
    optAct = new QAction(this);
    connect(optAct, &QAction::triggered, this, [this](bool on) {
        Q_UNUSED(on);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    groupWidgetTest->addOptionAction(optAct);

    groupWidgetTest->setVisible(true);
}

void MainWindow::createPageOther(RibbonPage *page)
{
    RibbonGroup *group1 = new RibbonGroup(tr("group one"));
    group1->setObjectName("PageOther-group1");
    page->addGroup(group1);
    // 按钮组
    RibbonButtonGroup *btnGroup1 = new RibbonButtonGroup(group1);
    btnGroup1->addAction(createAction(tr("Decrease Margin"), ":/icon/res/Decrease-Margin.svg"));
    btnGroup1->addAction(createAction(tr("Decrease Indent"), ":/icon/res/Decrease-Indent.svg"));
    btnGroup1->addAction(createAction(tr("Wrap Image Left"), ":/icon/res/Wrap-Image Left.svg"));
    btnGroup1->addAction(createAction(tr("Wrap Image Right"), ":/icon/res/Wrap-Image Right.svg"));
    group1->addWidget(btnGroup1, RibbonGroup::Medium);
    RibbonButtonGroup *btnGroup2 = new RibbonButtonGroup(group1);
    QAction *titleAlgnment = createAction(tr("Align Right"), ":/icon/res/Align-Right.svg");
    titleAlgnment->setProperty("align", (int)Qt::AlignRight | Qt::AlignVCenter);
    btnGroup2->addAction(titleAlgnment);
    titleAlgnment = createAction(tr("Align Left"), ":/icon/res/Align-Left.svg");
    titleAlgnment->setProperty("align", (int)Qt::AlignLeft | Qt::AlignVCenter);
    btnGroup2->addAction(titleAlgnment);
    titleAlgnment = createAction(tr("Align Center"), ":/icon/res/Align-Center.svg");
    titleAlgnment->setProperty("align", (int)Qt::AlignCenter);
    btnGroup2->addAction(titleAlgnment);
    group1->addWidget(btnGroup2, RibbonGroup::Medium);
    connect(btnGroup2, &RibbonButtonGroup::actionTriggered, this, &MainWindow::onButtonGroupActionTriggered);
    // Gallery
    RibbonGallery *gallery = group1->addGallery();
    QList<QAction *> galleryActions;
    auto lambdaCreateGalleryAction = [this](const QString &text, const QString &iconurl) -> QAction * {
        QAction *act = this->createAction(text, iconurl);
        this->connect(act, &QAction::triggered, this, [this, text]() {
            if (this->m_edit) {
                this->m_edit->append(QString("%1 triggered").arg(text));
            }
        });
        return act;
    };
    galleryActions.append(lambdaCreateGalleryAction("Document File", ":/gallery-icon/res/gallery/Document-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Download File", ":/gallery-icon/res/gallery/Download-File.svg"));
    galleryActions.append(
        lambdaCreateGalleryAction("Drive File Four Word", ":/gallery-icon/res/gallery/Drive-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Dropbox File", ":/gallery-icon/res/gallery/Dropbox-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Email File", ":/gallery-icon/res/gallery/Email-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Encode File", ":/gallery-icon/res/gallery/Encode-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Favorit File", ":/gallery-icon/res/gallery/Favorit-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("File Error", ":/gallery-icon/res/gallery/File-Error.svg"));
    galleryActions.append(lambdaCreateGalleryAction("File Read Only", ":/gallery-icon/res/gallery/File-Readonly.svg"));
    galleryActions.append(lambdaCreateGalleryAction("File Settings", ":/gallery-icon/res/gallery/File-Settings.svg"));
    galleryActions.append(
        lambdaCreateGalleryAction("Presentation File", ":/gallery-icon/res/gallery/Presentation-File.svg"));
    RibbonGalleryGroup *galleryGroup1 = gallery->addCategoryActions(tr("Files"), galleryActions);
    galleryGroup1->setGalleryGroupStyle(RibbonGalleryGroup::IconWithWordWrapText);
    galleryGroup1->setGridMinimumWidth(80);
    galleryActions.clear();
    galleryActions.append(lambdaCreateGalleryAction("Photoshop", ":/gallery-icon/res/gallery/Photoshop.svg"));
    galleryActions.append(
        lambdaCreateGalleryAction("Internet-Explorer", ":/gallery-icon/res/gallery/Internet-Explorer.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Illustrator", ":/gallery-icon/res/gallery/Illustrator.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Google-Maps", ":/gallery-icon/res/gallery/Google-Maps.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Adobe", ":/gallery-icon/res/gallery/Adobe.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Word", ":/gallery-icon/res/gallery/Word.svg"));
    RibbonGalleryGroup *galleryGroup2 = gallery->addCategoryActions(tr("Apps"), galleryActions);
    galleryGroup2->setGridMinimumWidth(80);
    gallery->setCurrentViewGroup(galleryGroup1);

    QAction *optAct = new QAction(this);

    optAct->setObjectName(("debug"));
    group1->addOptionAction(optAct);

    RibbonGroup *group2 = new RibbonGroup(tr("group two"));
    group2->setObjectName("PageOther-group2");
    page->addGroup(group2);
    QAction *actionRemoveAppBtn = createAction(tr("remove application button"), ":/icon/res/remove-app-btn.svg");
    actionRemoveAppBtn->setCheckable(true);
    connect(actionRemoveAppBtn, &QAction::toggled, this, &MainWindow::onActionRemoveAppBtnTriggered);
    group2->addLargeAction(actionRemoveAppBtn);

    QAction *actionLongText =
        createAction(tr("show very long text in a button,balabalabala etc"), ":/icon/res/long-text.svg", "long-text");
    group2->addLargeAction(actionLongText);

    RibbonGroup *groupStyle = new RibbonGroup(tr("style"));
    groupStyle->setObjectName("PageOther-groupStyle");
    page->addGroup(groupStyle);

    QAction *actionUseQss = createAction(tr("use qss"), ":/icon/res/useqss.svg");
    connect(actionUseQss, &QAction::triggered, this, &MainWindow::onActionUseQssTriggered);
    groupStyle->addSmallAction(actionUseQss);

    QAction *actionLoadCustomizeXmlFile =
        createAction(tr("load customize from xml file"), ":/icon/res/useCustomize.svg");
    connect(actionLoadCustomizeXmlFile, &QAction::triggered, this, &MainWindow::onActionLoadCustomizeXmlFileTriggered);
    groupStyle->addSmallAction(actionLoadCustomizeXmlFile);

    QAction *actionWindowFlagNormalButton =
        createAction(tr("window normal button"), ":/icon/res/windowsflag-normal.svg");
    actionWindowFlagNormalButton->setCheckable(true);
    actionWindowFlagNormalButton->setChecked(true);
    groupStyle->addSmallAction(actionWindowFlagNormalButton);
    connect(actionWindowFlagNormalButton, &QAction::triggered, this,
            &MainWindow::onActionWindowFlagNormalButtonTriggered);

    RibbonGroup *groupUtf8 = new RibbonGroup(QStringLiteral(u"中文显示测试"));
    groupUtf8->setObjectName("PageOther-groupUtf8");
    page->addGroup(groupUtf8);

    QAction *actionChangeText =
        createAction(QStringLiteral(u"改变显示为英文"), ":/icon/res/chinese-char.svg", "actionChangeText");
    actionChangeText->setCheckable(true);
    actionChangeText->setChecked(false);
    groupUtf8->addLargeAction(actionChangeText);
    connect(actionChangeText, &QAction::triggered, this, [groupUtf8, actionChangeText](bool on) {
        if (on) {
            groupUtf8->setGroupName(QStringLiteral(u"show chinese char"));
            actionChangeText->setText(QStringLiteral(u"show in chinese"));
        } else {
            groupUtf8->setGroupName(QStringLiteral(u"中文显示测试"));
            actionChangeText->setText(QStringLiteral(u"改变显示为英文"));
        }
    });

    RibbonGroup *groupContextPage = new RibbonGroup(("Context Page"));
    page->addGroup(groupContextPage);
    QAction *a1 = createAction("Context Page 1", ":/icon/res/ContextPage.svg");
    a1->setCheckable(true);
    connect(a1, &QAction::triggered, this, [this](bool c) {
        this->ribbonBar()->setPageContextVisible(m_pageContext, c);
    });

    QAction *a2 = createAction("Context Page 2", ":/icon/res/ContextPage.svg");
    a2->setCheckable(true);
    connect(a2, &QAction::triggered, this, [this](bool c) {
        this->ribbonBar()->setPageContextVisible(m_pageContext2, c);
    });
    groupContextPage->addLargeAction(a1);
    groupContextPage->addLargeAction(a2);
}

void MainWindow::createPageDelete(RibbonPage *page)
{
    RibbonGroup *group1 = new RibbonGroup(("group 1"));
    RibbonGroup *group2 = new RibbonGroup(("group 2"));

    QAction *actionRemovePanne = createAction(tr("remove group"), ":/icon/res/remove.svg");

    connect(actionRemovePanne, &QAction::triggered, this, [page, group2]() {
        page->removeGroup(group2);
    });
    group1->addLargeAction(actionRemovePanne);

    QAction *actionRemovePage = createAction(tr("remove this page"), ":/icon/res/removePage.svg");

    connect(actionRemovePage, &QAction::triggered, this, [this, page, actionRemovePage]() {
        this->ribbonBar()->removePage(page);
        page->hide();
        page->deleteLater();
        actionRemovePage->setDisabled(true);
    });
    group2->addLargeAction(actionRemovePage);

    page->addGroup(group1);
    page->addGroup(group2);
}

/**
 * @brief 构建尺寸页
 * @param page
 */
void MainWindow::createPageSize(RibbonPage *page)
{
    QAction *act = nullptr;
    RibbonGroup *group = page->addGroup(tr("Font"));
    RibbonButtonGroup *group1 = new RibbonButtonGroup(group);
    group1->setObjectName(QStringLiteral(u"group1"));
    QLabel *labelFontSize = new QLabel(group1);
    labelFontSize->setText(tr("select font"));
    labelFontSize->setObjectName(QStringLiteral(u"labelFontSize"));
    group1->addWidget(labelFontSize);
    QFontComboBox *fontComWidget = new QFontComboBox(group1);
    fontComWidget->setObjectName(QStringLiteral(u"fontComboBox"));
    connect(fontComWidget, &QFontComboBox::currentFontChanged, this, &MainWindow::onFontComWidgetCurrentFontChanged);
    group1->addWidget(fontComWidget);
    act = group->addWidget(group1, RibbonGroup::Medium);
    act->setObjectName(labelFontSize->objectName());

    RibbonButtonGroup *group2 = new RibbonButtonGroup(group);
    group2->setObjectName(QStringLiteral(u"group2"));
    group2->addAction(createAction("Bold", ":/icon/res/bold.svg"));
    group2->addAction(createAction("Italic", ":/icon/res/Italic.svg"));
    group2->addSeparator();
    group2->addAction(createAction("left alignment", ":/icon/res/al-left.svg"));
    group2->addAction(createAction("center alignment", ":/icon/res/al-center.svg"));
    group2->addAction(createAction("right alignment", ":/icon/res/al-right.svg"));
    group2->addAction(createAction("line up on both sides", ":/icon/res/al-bothside.svg"));
    act = group->addWidget(group2, RibbonGroup::Medium);
    act->setObjectName(group2->objectName());

    group->addSeparator();

    QAction *actLargerFontSize = createAction(tr("Larger"), ":/icon/res/largerFont.svg", "actLargerFontSize");
    group->addLargeAction(actLargerFontSize);

    QAction *actSmallFontSize = createAction(tr("Smaller"), ":/icon/res/smallFont.svg", "actSmallFontSize");
    group->addLargeAction(actSmallFontSize);

    connect(actLargerFontSize, &QAction::triggered, this, &MainWindow::onActionFontLargerTriggered);
    connect(actSmallFontSize, &QAction::triggered, this, &MainWindow::onActionFontSmallerTriggered);
}

/**
 * @brief 创建上下文标签
 */
void MainWindow::createPageContext1()
{
    // 上下文标签
    RibbonBar *ribbon = ribbonBar();
    m_pageContext = ribbon->addPageContext(tr("context"), QColor(), 1);

    RibbonPage *contextPagePage1 = m_pageContext->addPage(tr("Page1"));
    createPageContext1(contextPagePage1);

    RibbonPage *contextPagePage2 = m_pageContext->addPage(tr("Page2"));
    createPageContext2(contextPagePage2);
}

void MainWindow::createPageContext2()
{
    RibbonBar *ribbon = ribbonBar();
    m_pageContext2 = ribbon->addPageContext(("context2"), QColor(), 2);
    m_pageContext2->addPage(("context2 Page1"));
    m_pageContext2->addPage(("context2 Page2"));
}

void MainWindow::createPageContext1(RibbonPage *page)
{
    RibbonGroup *group = page->addGroup(tr("show and hide test"));

    QAction *actionHideGroup = createAction("hide group", ":/icon/res/hideGroup.svg");
    actionHideGroup->setCheckable(true);
    group->addLargeAction(actionHideGroup);

    QAction *actionDisable = createAction(tr("Disable"), ":/icon/res/enableTest.svg");

    actionDisable->setDisabled(true);
    group->addLargeAction(actionDisable);
    connect(actionDisable, &QAction::triggered, this, [actionDisable](bool b) {
        Q_UNUSED(b);
        actionDisable->setDisabled(true);
    });

    QAction *actionUnlock = createAction(tr("unlock"), ":/icon/res/unlock.svg");
    actionUnlock->setShortcut(QKeySequence(QLatin1String("Ctrl+E")));
    group->addLargeAction(actionUnlock);
    connect(actionUnlock, &QAction::triggered, this, [actionDisable](bool b) {
        Q_UNUSED(b);
        actionDisable->setEnabled(true);
        actionDisable->setText(("Enabled"));
    });

    QAction *actionSetTextTest = createAction("set text", ":/icon/res/setText.svg");

    actionSetTextTest->setCheckable(true);
    actionSetTextTest->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    group->addLargeAction(actionSetTextTest);

    connect(actionSetTextTest, &QAction::toggled, this, [actionSetTextTest](bool b) {
        if (b) {
            actionSetTextTest->setText(QStringLiteral(u"setText测试"));
        } else {
            actionSetTextTest->setText(QStringLiteral(u"set text"));
        }
    });
    // 隐藏group
    QAction *actionShowTest = createAction("show beside group", ":/icon/res/show.svg");

    actionShowTest->setCheckable(true);
    group->addLargeAction(actionShowTest);

    RibbonGroup *group2 = page->addGroup(tr("show/hide"));

    group2->addLargeAction(actionSetTextTest);

    connect(actionShowTest, &QAction::toggled, this, [actionShowTest, group2, this](bool b) {
        group2->setVisible(!b);
        if (b) {
            actionShowTest->setText(tr("hide beside group"));
        } else {
            actionShowTest->setText(tr("show beside group"));
        }
        ribbonBar()->repaint();
    });

    RibbonGroup *group3 = page->addGroup(("show/hide action test"));

    QAction *actionHideAction2 = createAction("hide action 2", ":/icon/res/action.svg");
    QAction *actionHideAction4 = createAction("hide action 4", ":/icon/res/action.svg");
    actionHideAction2->setCheckable(true);
    actionHideAction2->setChecked(true);
    actionHideAction4->setCheckable(true);
    actionHideAction4->setChecked(true);
    QAction *act2 = createAction("action2", ":/icon/res/action2.svg");
    QAction *act3 = createAction("action3", ":/icon/res/action3.svg");
    QAction *act4 = createAction("action4", ":/icon/res/action4.svg");
    group3->addLargeAction(actionHideAction2);
    group3->addLargeAction(actionHideAction4);
    group3->addSmallAction(act2);
    group3->addSmallAction(act3);
    group3->addSmallAction(act4);
    connect(actionHideAction2, &QAction::triggered, this, [actionHideAction2, act2](bool b) {
        if (b) {
            act2->setVisible(true);
            actionHideAction2->setText(tr("hide action2"));
        } else {
            act2->setVisible(false);
            actionHideAction2->setText(tr("show action2"));
        }
    });
    connect(actionHideAction4, &QAction::triggered, this, [actionHideAction4, act4](bool b) {
        if (b) {
            act4->setVisible(true);
            actionHideAction4->setText(tr("hide action4"));
        } else {
            act4->setVisible(false);
            actionHideAction4->setText(tr("show action4"));
        }
    });
    //
    RibbonGroup *group4 = page->addGroup(("widget"));
    QSpinBox *spinbox = new QSpinBox(this);
    RibbonCtrlContainer *ctrlContainer1 = new RibbonCtrlContainer(group4);
    ctrlContainer1->setWidget(spinbox);
    ctrlContainer1->setText(tr("spinbox:"));
    ctrlContainer1->setIconVisible(false);
    group4->addMediumWidget(ctrlContainer1);

    QLineEdit *linedit = new QLineEdit(this);
    RibbonCtrlContainer *ctrlContainer2 = new RibbonCtrlContainer(group4);
    ctrlContainer2->setWidget(linedit);
    ctrlContainer2->setText(tr("linedit:"));
    ctrlContainer2->setIconVisible(false);
    group4->addMediumWidget(ctrlContainer2);
}

void MainWindow::createPageContext2(RibbonPage *page)
{
    RibbonGroup *group1 = page->addGroup(QStringLiteral(u"删除PagePage测试"));

    QAction *actionDeleteThisPage = createAction("delete this page", ":/icon/res/delete.svg");
    group1->addLargeAction(actionDeleteThisPage);
    connect(actionDeleteThisPage, &QAction::triggered, this, [this, page]() {
        this->ribbonBar()->removePage(page);
        page->deleteLater();
    });
    RibbonGroup *groupLayout = page->addGroup(QStringLiteral(u"特殊布局"));

    groupLayout->addAction(createAction("Large", ":/icon/res/layout.svg", "@Large1"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addAction(createAction("Small", ":/icon/res/layout.svg", "@Small1"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction("Small", ":/icon/res/layout.svg", "@Small2"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addSeparator();
    groupLayout->addAction(createAction("Small", ":/icon/res/layout.svg", "@Small3"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction("Small", ":/icon/res/layout.svg", "@Small4"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction("Small", ":/icon/res/layout.svg", "@Small5"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction("Small", ":/icon/res/layout.svg", "@Small6"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addSeparator();
    groupLayout->addAction(createAction("Large", ":/icon/res/layout.svg", "@Large2"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addAction(createAction("Medium", ":/icon/res/layout.svg", "@Medium1"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction("Medium", ":/icon/res/layout.svg", "@Medium2"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction("Small", ":/icon/res/layout.svg", "@Small7"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction("Medium", ":/icon/res/layout.svg", "@Medium3"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction("Large", ":/icon/res/layout.svg", "@Large3"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addAction(createAction("Medium", ":/icon/res/layout.svg", "@Medium4"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction("Medium", ":/icon/res/layout.svg", "@Medium5"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction("Large", ":/icon/res/layout.svg", "@Large4"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addSeparator();
    groupLayout->addAction(createAction("Medium", ":/icon/res/layout.svg", "@Medium6"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction("Large", ":/icon/res/layout.svg", "@Large5"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addAction(createAction("Medium", ":/icon/res/layout.svg", "@Medium7"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction("Small", ":/icon/res/layout.svg", "@Small8"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
}

void MainWindow::createQuickAccessBar(RibbonQuickAccessBar *quickAccessBar)
{
    quickAccessBar->addAction(createAction("save", ":/icon/res/save.svg", "save-quickbar"));
    quickAccessBar->addSeparator();
    quickAccessBar->addAction(createAction("undo", ":/icon/res/undo.svg"));
    quickAccessBar->addAction(createAction("redo", ":/icon/res/redo.svg"));
    quickAccessBar->addSeparator();
    QMenu *m = new QMenu("Presentation File", this);
    m->setIcon(QIcon(":/icon/res/presentationFile.svg"));
    for (int i = 0; i < 10; ++i) {
        m->addAction(createAction(QString("file%1").arg(i + 1), ":/icon/res/file.svg"));
    }
    quickAccessBar->addMenu(m);

    QAction *customize = createAction("customize", ":/icon/res/customize0.svg", "customize2");
    quickAccessBar->addAction(customize);
    connect(customize, &QAction::triggered, this, &MainWindow::onActionCustomizeTriggered);

    QAction *actionCustomizeAndSave = createAction("customize and save", ":/icon/res/customize.svg");
    quickAccessBar->addAction(actionCustomizeAndSave);
    connect(actionCustomizeAndSave, &QAction::triggered, this, &MainWindow::onActionCustomizeAndSaveTriggered);
}

void MainWindow::createRightButtonGroup(RibbonButtonGroup *rightBar)
{
    QAction *actionHelp = createAction(tr("help"), ":/icon/res/help.svg");
    connect(actionHelp, &QAction::triggered, this, &MainWindow::onActionHelpTriggered);
    rightBar->addAction(actionHelp);
}

void MainWindow::addSomeOtherAction()
{
    // 添加其他的action，这些action并不在ribbon管理范围，主要用于RibbonCustomizeWidget自定义用
    QAction *acttext1 = new QAction(("text action1"), this);
    QAction *acttext2 = new QAction(("text action2"), this);
    QAction *acttext3 = new QAction(("text action3"), this);
    QAction *acttext4 = new QAction(("text action4"), this);
    QAction *acttext5 = new QAction(("text action5"), this);

    QAction *actIcon1 = new QAction(QIcon(":/icon/res/layout.svg"), ("action with icon"), this);

    m_actionTagText = RibbonActionsManager::UserDefineActionTag + 1;
    m_actionTagWithIcon = RibbonActionsManager::UserDefineActionTag + 2;

    m_actMgr = new RibbonActionsManager(this);   // 申明过程已经自动注册所有action

    // 以下注册特别的action
    m_actMgr->registeAction(acttext1, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(acttext3, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(acttext5, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(actIcon1, RibbonActionsManager::CommonlyUsedActionTag);

    m_actMgr->registeAction(acttext1, m_actionTagText);
    m_actMgr->registeAction(acttext2, m_actionTagText);
    m_actMgr->registeAction(acttext3, m_actionTagText);
    m_actMgr->registeAction(acttext4, m_actionTagText);
    m_actMgr->registeAction(acttext5, m_actionTagText);

    m_actMgr->registeAction(actIcon1, m_actionTagWithIcon);

    m_actMgr->setTagName(RibbonActionsManager::CommonlyUsedActionTag, tr("in common use"));
    m_actMgr->setTagName(m_actionTagText, tr("no icon action"));
    m_actMgr->setTagName(m_actionTagWithIcon, tr("have icon action"));
}

QAction *MainWindow::createAction(const QString &text, const QString &iconurl, const QString &objName)
{
    QAction *act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(objName);
    return act;
}

QAction *MainWindow::createAction(const QString &text, const QString &iconurl)
{
    QAction *act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(text);
    return act;
}

void MainWindow::onMenuButtonPopupCheckableTest(bool b)
{
    m_edit->append(QString("MenuButtonPopupCheckableTest : %1").arg(b));
}

void MainWindow::onInstantPopupCheckableTest(bool b)
{
    m_edit->append(QString("InstantPopupCheckableTest : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckableTest(bool b)
{
    m_edit->append(QString("DelayedPopupCheckableTest : %1").arg(b));
}

void MainWindow::onMenuButtonPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("MenuButtonPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onInstantPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("InstantPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("DelayedPopupCheckabletriggered : %1").arg(b));
}
