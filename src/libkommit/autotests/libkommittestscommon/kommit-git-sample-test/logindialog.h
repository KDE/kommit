#pragma once

#include "ui_logindialog.h"

class LoginDialog : public QDialog, private Ui::LoginDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
private slots:
    void on_buttonBox_accepted();
};
