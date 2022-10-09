/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_selectbranchestodiffdialog.h"

namespace Git
{
class Manager;
}

class SelectBranchesToDiffDialog : public AppDialog, private Ui::SelectBranchesToDiffDialog
{
    Q_OBJECT

public:
    explicit SelectBranchesToDiffDialog(Git::Manager *git, QWidget *parent = nullptr);

    QString oldBranch() const;
    QString newBranch() const;

private Q_SLOTS:
    void on_buttonBox_accepted();
};
