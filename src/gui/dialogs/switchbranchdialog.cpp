/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "switchbranchdialog.h"
#include "commands/commandswitchbranch.h"
#include "gitmanager.h"
#include "models/tagsmodel.h"

#define BRANCH_TYPE_LOCAL 1
#define BRANCH_TYPE_REMOTE 2

SwitchBranchDialog::SwitchBranchDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    auto localBranches = git->branches();
    for (const auto &b : localBranches)
        comboBoxBranchSelect->addItem(b, BRANCH_TYPE_LOCAL);
    auto remoteBranches = git->remoteBranches();
    for (const auto &b : remoteBranches)
        comboBoxBranchSelect->addItem(b, BRANCH_TYPE_REMOTE);

    comboBoxTags->setModel(git->tagsModel());

    radioButtonTag->setEnabled(comboBoxTags->count());
}

Git::CommandSwitchBranch *SwitchBranchDialog::command() const
{
    auto cmd = new Git::CommandSwitchBranch(mGit);

    if (radioButtonExistingBranch->isChecked()) {
        cmd->setMode(comboBoxBranchSelect->currentData().toInt() == BRANCH_TYPE_LOCAL ? Git::CommandSwitchBranch::ExistingBranch
                                                                                      : Git::CommandSwitchBranch::RemoteBranch);
        cmd->setTarget(comboBoxBranchSelect->currentText());
    } else if (radioButtonTag->isChecked()) {
        cmd->setMode(Git::CommandSwitchBranch::Tag);
        cmd->setTarget(comboBoxTags->currentText());
    } else {
        cmd->setMode(Git::CommandSwitchBranch::NewBranch);
        cmd->setTarget(lineEditNewBranchName->text());
    }

    return cmd;
}

// void SwitchBranchDialog::slotButtonBoxAccepted()
//{
//     Git::CommandSwitchBranch cmd(_git);

//    if (radioButtonExistingBranch->isChecked()) {
//        cmd.setMode(Git::CommandSwitchBranch::ExistingBranch);
//        cmd.setTarget(comboBoxBranchSelect->currentText());
//    } else if (radioButtonTag->isChecked()) {
//        cmd.setMode(Git::CommandSwitchBranch::Tag);
//        cmd.setTarget(comboBoxTags->currentText());
//    } else {
//        cmd.setMode(Git::CommandSwitchBranch::NewBranch);
//        cmd.setTarget(lineEditNewBranchName->text());
//    }

//    qCDebug(GITKLIENT_LOG) << cmd.target();
//    return;

//    cmd.setForce(checkBoxForce->isChecked());

//    RunnerDialog d(this);
//    d.run(&cmd);
//    d.exec();

//    accept();
//}
