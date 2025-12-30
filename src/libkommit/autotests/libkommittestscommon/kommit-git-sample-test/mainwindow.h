#pragma once

#include "./ui_mainwindow.h"

#include <QMainWindow>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
