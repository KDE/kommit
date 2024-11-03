/*
SPDX-FileCopyrightText: 2023 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "credentialdialog.h"

CredentialDialog::CredentialDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

QString CredentialDialog::password() const
{
    return lineEditPassword->text();
}

void CredentialDialog::setPassword(const QString &password)
{
    lineEditPassword->setText(password);
}

QString CredentialDialog::url() const
{
    return mUrl;
}

void CredentialDialog::setUrl(const QString &url)
{
    mUrl = url;
    labelCredentialsUrl->setText(i18n("Credential for %1", url));
}

QString CredentialDialog::username() const
{
    return lineEditUsername->text();
}

void CredentialDialog::setUsername(const QString &username)
{
    lineEditUsername->setText(username);
}

#include "moc_credentialdialog.cpp"
