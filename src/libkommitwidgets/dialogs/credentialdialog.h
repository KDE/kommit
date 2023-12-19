#pragma once

#include "ui_credentialdialog.h"

class CredentialDialog : public QDialog, private Ui::CredentialDialog
{
    Q_OBJECT

public:
    explicit CredentialDialog(QWidget *parent = nullptr);

    Q_REQUIRED_RESULT QString username() const;
    void setUsername(const QString &username);

    Q_REQUIRED_RESULT QString password() const;
    void setPassword(const QString &password);

    Q_REQUIRED_RESULT QString url() const;
    void setUrl(const QString &url);

private:
    QString mUrl;
};
