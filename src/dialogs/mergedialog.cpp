/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergedialog.h"

#include <QWhatsThisClickedEvent>

#include "git/commands/commandmerge.h"
#include "git/gitmanager.h"
#include "git/models/branchesmodel.h"

void MergeDialog::init(Git::Manager *git)
{
    comboBoxBranchName->setModel(git->branchesModel());
    labelToBranchName->setText(git->currentBranch());

    checkBoxAllowUnrelatedHistories->setCheckState(Qt::PartiallyChecked);
    checkBoxSquash->setCheckState(Qt::PartiallyChecked);
}

MergeDialog::MergeDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    init(git);
}

MergeDialog::MergeDialog(Git::Manager *git, const QString &sourceBranch, QWidget *parent)
    : AppDialog(git, parent)
    , mSourceBranch(sourceBranch)
{
    setupUi(this);

    init(git);

    comboBoxBranchName->setCurrentText(sourceBranch);
}

Git::CommandMerge *MergeDialog::command() const
{
    auto cmd = new Git::CommandMerge(mGit);

    cmd->setAllowUnrelatedHistories(Git::checkStateToOptionalBool(checkBoxAllowUnrelatedHistories->checkState()));
    cmd->setSquash(checkBoxSquash->isChecked());

    if (radioButtonAllowFastForward->isChecked())
        cmd->setFf(Git::FastForwardType::Yes);
    else if (radioButtonFastForwardOnly->isChecked())
        cmd->setFf(Git::FastForwardType::OnlyFastForward);
    else if (radioButtonNofastForward->isChecked())
        cmd->setFf(Git::FastForwardType::No);
    else
        cmd->setFf(Git::FastForwardType::Unset);

    if (comboBoxStrategy->currentIndex() > 0)
        cmd->setStrategy(comboBoxStrategy->currentText().toLower());

    return cmd;
}
