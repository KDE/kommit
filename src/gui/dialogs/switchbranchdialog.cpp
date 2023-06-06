/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "switchbranchdialog.h"
#include "commands/commandswitchbranch.h"
#include "gitmanager.h"
#include "kommit_appdebug.h"
#include "models/tagsmodel.h"

#include <QDebug>

#include <KLocalizedString>
#include <KMessageBox>

#define BRANCH_TYPE_LOCAL 1
#define BRANCH_TYPE_REMOTE 2

SwitchBranchDialog::SwitchBranchDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    _existingLocalBranches = git->branches();
    for (const auto &b : std::as_const(_existingLocalBranches))
        comboBoxBranchSelect->addItem(b, BRANCH_TYPE_LOCAL);
    _existingRemoteBranches = git->remoteBranches();
    for (const auto &b : std::as_const(_existingRemoteBranches))
        comboBoxBranchSelect->addItem(b, BRANCH_TYPE_REMOTE);

    comboBoxTags->setModel(git->tagsModel());

    radioButtonTag->setEnabled(comboBoxTags->count());
}

Git::CommandSwitchBranch *SwitchBranchDialog::command() const
{
    auto cmd = new Git::CommandSwitchBranch(mGit);

    if (radioButtonExistingBranch->isChecked()) {
        if (comboBoxBranchSelect->currentData().toInt() == BRANCH_TYPE_LOCAL) {
            cmd->setTarget(comboBoxBranchSelect->currentText());
            cmd->setMode(Git::CommandSwitchBranch::ExistingBranch);
        } else {
            const auto n = comboBoxBranchSelect->currentText().indexOf(QLatin1Char('/'));
            if (n == -1) {
                qCWarning(KOMMIT_LOG, "The branch name %s is invalid", qPrintable(comboBoxBranchSelect->currentText()));
                cmd->deleteLater();
                return nullptr;
            }

            cmd->setRemoteBranch(comboBoxBranchSelect->currentText().mid(0, n));
            cmd->setTarget(comboBoxBranchSelect->currentText().mid(n + 1));
            cmd->setMode(Git::CommandSwitchBranch::RemoteBranch);
        }
    } else if (radioButtonTag->isChecked()) {
        cmd->setMode(Git::CommandSwitchBranch::Tag);
        cmd->setTarget(comboBoxTags->currentText());
    } else {
        cmd->setMode(Git::CommandSwitchBranch::NewBranch);
        cmd->setTarget(lineEditNewBranchName->text());
    }

    return cmd;
}

void SwitchBranchDialog::on_buttonBox_accepted()
{
    if (radioButtonExistingBranch->isChecked())
        if (_existingLocalBranches.contains(lineEditNewBranchName->text()) || _existingRemoteBranches.contains(lineEditNewBranchName->text())) {
            KMessageBox::error(this, i18n("The branch already exists"));
            return;
        }
    accept();
}
