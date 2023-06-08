#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    setWindowIcon(QIcon(":/res/image/logo.png"));
    setWindowTitle(tr("QxRibbon Library Template"));

    resize(400, 200);
}

MainWindow::~MainWindow()
{

}
