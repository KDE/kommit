#include "logindialog.h"

#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

void LoginDialog::on_buttonBox_accepted()
{
    if (lineEditUsername->text() == "admin" && lineEditPassword->text() == "123456")
        accept();
    else
        QMessageBox::warning(this, "Login", "Username/Password is incorrect");
}
