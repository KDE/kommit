/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_switchbranchdialog.h"

namespace Git
{
class CommandSwitchBranch;
}
class LIBKOMMITWIDGETS_EXPORT SwitchBranchDialog : public AppDialog, private Ui::SwitchBranchDialog
{
    Q_OBJECT

public:
    explicit SwitchBranchDialog(Git::Repository *git, QWidget *parent = nullptr);

    [[nodiscard]] Git::CommandSwitchBranch *command() const;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotButtonBoxAccepted();
    QStringList mExistingLocalBranches;
    QStringList mExistingRemoteBranches;
};
