/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_switchbranchdialog.h"

namespace Git
{
class CommandSwitchBranch;
}
class SwitchBranchDialog : public AppDialog, private Ui::SwitchBranchDialog
{
    Q_OBJECT

public:
    explicit SwitchBranchDialog(Git::Manager *git, QWidget *parent = nullptr);

    Git::CommandSwitchBranch *command() const;

private Q_SLOTS:
    //    void on_buttonBox_accepted();
};
