/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "switchbranchdialog.h"
#include "commands/commandswitchbranch.h"
#include "gitmanager.h"
#include "models/branchesmodel.h"
#include "models/tagsmodel.h"
#include "runnerdialog.h"

#include "gitklient_appdebug.h"

SwitchBranchDialog::SwitchBranchDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    comboBoxBranchSelect->addItems(git->branches());
    comboBoxBranchSelect->addItems(git->remoteBranches());

    comboBoxTags->setModel(git->tagsModel());
}

Git::CommandSwitchBranch *SwitchBranchDialog::command() const
{
    auto cmd = new Git::CommandSwitchBranch(mGit);

    if (radioButtonExistingBranch->isChecked()) {
        cmd->setMode(Git::CommandSwitchBranch::ExistingBranch);
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

// void SwitchBranchDialog::on_buttonBox_accepted()
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
