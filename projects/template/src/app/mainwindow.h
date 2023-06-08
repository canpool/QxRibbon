#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QxRibbonWindow.h"

class MainWindow : public RibbonWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_H
