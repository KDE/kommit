/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_mergedialog.h"

namespace Git
{
class CommandMerge;
class Repository;
}
class LIBKOMMITWIDGETS_EXPORT MergeDialog : public AppDialog, private Ui::MergeDialog
{
    Q_OBJECT
public:
    explicit MergeDialog(Git::Repository *git, QWidget *parent = nullptr);
    explicit MergeDialog(Git::Repository *git, const QString &sourceBranch, QWidget *parent = nullptr);
    [[nodiscard]] Git::CommandMerge *command() const;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotComboBoxStrategyCurrentIndexChanged(int index);
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushButtonAnalyseClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void init(Git::Repository *git);
    const QString mSourceBranch;
};
