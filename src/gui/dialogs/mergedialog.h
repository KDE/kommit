/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_mergedialog.h"

namespace Git
{
class CommandMerge;
class Manager;
}
class MergeDialog : public AppDialog, private Ui::MergeDialog
{
    Q_OBJECT
public:
    explicit MergeDialog(Git::Manager *git, QWidget *parent = nullptr);
    explicit MergeDialog(Git::Manager *git, const QString &sourceBranch, QWidget *parent = nullptr);
    Git::CommandMerge *command() const;

private slots:
    void slotComboBoxStrategyCurrentIndexChanged(int index);

private:
    QString mSourceBranch;
    void init(Git::Manager *git);
};
