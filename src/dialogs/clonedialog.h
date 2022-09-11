/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../core/appdialog.h"
#include "ui_clonedialog.h"

#include <git/commands/commandclone.h>

class CloneDialog : public AppDialog, private Ui::CloneDialog
{
    Q_OBJECT

public:
    explicit CloneDialog(QWidget *parent = nullptr);
    ~CloneDialog();

    Git::CloneCommand *command();

    void setLocalPath(const QString &path);

private Q_SLOTS:
    void on_lineEditUrl_textChanged(const QString &text);
    void on_toolButtonBrowseLocalPath_clicked();
    void on_buttonBox_accepted();

private:
    QString mFixedPath;
};

