/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "cleanupdialog.h"
#include "commands/commandclean.h"

CleanupDialog::CleanupDialog(QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);
}

Git::CommandClean *CleanupDialog::command() const
{
    auto cmd = new Git::CommandClean;
    if (radioButtonAllUntrackedFiles->isChecked())
        cmd->setType(Git::CommandClean::AllUntrackedFiles);
    else if (radioButtonNonIgnoredUntrackedFiles->isChecked())
        cmd->setType(Git::CommandClean::NonIgnoredUntrackedFiles);
    else
        cmd->setType(Git::CommandClean::IgnoredFiles);

    cmd->setRemoveUntrackedDirectories(checkBoxRemoveDirectories->isChecked());

    return cmd;
}
