/*
SPDX-FileCopyrightText: 2023 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

#include "ui_credentialdialog.h"

class CredentialDialog : public QDialog, private Ui::CredentialDialog
{
    Q_OBJECT

public:
    explicit CredentialDialog(QWidget *parent = nullptr);

    [[nodiscard]] QString username() const;
    void setUsername(const QString &username);

    [[nodiscard]] QString password() const;
    void setPassword(const QString &password);

    [[nodiscard]] QString url() const;
    void setUrl(const QString &url);

private:
    QString mUrl;
};
