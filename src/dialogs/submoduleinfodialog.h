/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_submoduleinfodialog.h"

namespace Git
{
class AddSubmoduleCommand;
class Manager;
}
class SubmoduleInfoDialog : public AppDialog, private Ui::SubmoduleInfoDialog
{
    Q_OBJECT

public:
    explicit SubmoduleInfoDialog(Git::Manager *git, QWidget *parent = nullptr);

    bool force() const;
    void setForce(bool newForce);

    QString url() const;
    void setUrl(const QString &newUrl);

    QString path() const;
    void setPath(const QString &newPath);

    QString branch() const;
    void setBranch(const QString &newBranch);

    Git::AddSubmoduleCommand *command() const;

private Q_SLOTS:
    void on_toolButtonBrowseLocalPath_clicked();
};
