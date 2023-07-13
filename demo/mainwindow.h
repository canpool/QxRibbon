#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QxRibbonWindow.h"

class QTextEdit;
class QActionGroup;

class RibbonPage;
class RibbonPageContext;
class RibbonCustomizeWidget;
class RibbonActionsManager;
class RibbonQuickAccessBar;
class RibbonButtonGroup;

class MainWindow : public RibbonWindow
{
    Q_OBJECT
public:
    enum RibbonThemeCustom {
        Office2016BlueTheme = CustomTheme + 1,
    };
public:
    MainWindow(QWidget *par = nullptr);
    void setRibbonTheme(int theme);
private:
    void createCentralWidget();
    void createStatusBar();
    void createRibbon();

    void createPageHome();
    void createPageOther();
    void createPageCustom();
    void createPageApply();
    void createPageContext1();
    void createPageContext2();
    void createPageContext1(RibbonPage *page);
    void createPageContext2(RibbonPage *page);
    void createQuickAccessBar();
    void createRightButtonGroup();
    void addSomeOtherAction();

    QAction *createAction(const QString &text, const QString &iconurl, const QString &objName);
    QAction *createAction(const QString &text, const QString &iconurl);
    QAction *addThemeAction(QAction *action, int themeId);

    void changeRibbonTheme(int index);
private slots:
    void onMenuButtonPopupCheckableTest(bool b);
    void onInstantPopupCheckableTest(bool b);
    void onDelayedPopupCheckableTest(bool b);
    void onMenuButtonPopupCheckabletriggered(bool b);
    void onInstantPopupCheckabletriggered(bool b);
    void onDelayedPopupCheckabletriggered(bool b);
    void onShowContextPage(bool on);
    void onStyleClicked(int id);
    void onActionCustomizeTriggered(bool b);
    void onActionCustomizeAndSaveTriggered(bool b);
    void onActionHelpTriggered();
    void onActionRemoveAppBtnTriggered(bool b);
    void onActionUseQssTriggered();
    void onActionLoadCustomizeXmlFileTriggered();
    void onActionWindowFlagNormalButtonTriggered(bool b);
    void onActionChangeThemeTriggered();

    void onFontComWidgetCurrentFontChanged(const QFont &f);
    void onActionFontLargerTriggered();
    void onActionFontSmallerTriggered();
    void onActionwordWrapIn2rowTriggered(bool b);
    void onButtonGroupActionTriggered(QAction *act);
    void onApplicationButtonClicked();
private:
    RibbonPageContext *m_pageContext;
    RibbonPageContext *m_pageContext2;
    RibbonCustomizeWidget *m_customizeWidget;
    QTextEdit *m_edit;
    QActionGroup *m_themeGroup;
    RibbonActionsManager *m_actMgr;
    int m_actionTagText;
    int m_actionTagWithIcon;
};

#endif   // MAINWINDOW_H
