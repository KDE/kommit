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

    Q_REQUIRED_RESULT bool force() const;
    void setForce(bool newForce);

    Q_REQUIRED_RESULT QString url() const;
    void setUrl(const QString &newUrl);

    Q_REQUIRED_RESULT QString path() const;
    void setPath(const QString &newPath);

    Q_REQUIRED_RESULT QString branch() const;
    void setBranch(const QString &newBranch);

    Q_REQUIRED_RESULT Git::AddSubmoduleCommand *command() const;

private Q_SLOTS:
    void slotLineEditPathUrlSelected(const QUrl &url);
};
