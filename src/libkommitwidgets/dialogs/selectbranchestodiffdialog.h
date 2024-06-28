/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_selectbranchestodiffdialog.h"

namespace Git
{
class Manager;
}

class LIBKOMMITWIDGETS_EXPORT SelectBranchesToDiffDialog : public AppDialog, private Ui::SelectBranchesToDiffDialog
{
    Q_OBJECT

public:
    explicit SelectBranchesToDiffDialog(Git::Manager *git, QWidget *parent = nullptr);

    [[nodiscard]] QString oldBranch() const;
    [[nodiscard]] QString newBranch() const;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotAccepted();
};
