/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "switchbranchdialog.h"
#include <caches/branchescache.h>
#include <caches/referencecache.h>
#include <caches/tagscache.h>
#include <commands/commandswitchbranch.h>
#include <repository.h>

#include "core/kmessageboxhelper.h"
#include "libkommitwidgets_appdebug.h"

#include <KLocalizedString>

#define BRANCH_TYPE_LOCAL 1
#define BRANCH_TYPE_REMOTE 2

SwitchBranchDialog::SwitchBranchDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    mExistingLocalBranches = git->branches()->names(Git::BranchType::LocalBranch);
    for (const auto &b : std::as_const(mExistingLocalBranches))
        comboBoxBranchSelect->addItem(b, BRANCH_TYPE_LOCAL);
    mExistingRemoteBranches = git->branches()->names(Git::BranchType::RemoteBranch);
    for (const auto &b : std::as_const(mExistingRemoteBranches))
        comboBoxBranchSelect->addItem(b, BRANCH_TYPE_REMOTE);

    comboBoxTags->addItems(git->tags()->allNames());

    radioButtonTag->setEnabled(comboBoxTags->count());

    connect(buttonBox, &QDialogButtonBox::accepted, this, &SwitchBranchDialog::slotButtonBoxAccepted);
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
                qCWarning(KOMMIT_WIDGETS_LOG, "The branch name %s is invalid", qPrintable(comboBoxBranchSelect->currentText()));
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

void SwitchBranchDialog::slotButtonBoxAccepted()
{
    if (radioButtonNewBranch->isChecked())
        if (mExistingLocalBranches.contains(lineEditNewBranchName->text()) || mExistingRemoteBranches.contains(lineEditNewBranchName->text())) {
            KMessageBoxHelper::error(this, i18n("The branch already exists"));
            return;
        }

    bool switched{};

    if (radioButtonExistingBranch->isChecked()) {
        auto branch = mGit->branches()->findByName(comboBoxBranchSelect->currentText());
        switched = mGit->switchBranch(branch);
    } else if (radioButtonTag->isChecked()) {
        auto tag = mGit->tags()->find(comboBoxTags->currentText());
        auto commit = tag->commit();

        if (!commit) {
            KMessageBoxHelper::error(this, i18n("Unable to find the tag's target commit: %1", comboBoxTags->currentText()));
            return;
        }
        switched = mGit->reset(commit, Git::Repository::ResetType::Hard);
    } else {
        if (!mGit->branches()->create(lineEditNewBranchName->text())) {
            KMessageBoxHelper::error(this, i18n("Unable to create the branch: %1", lineEditNewBranchName->text()));
            return;
        }
        auto newBranch = mGit->branches()->findByName(lineEditNewBranchName->text());
        switched = mGit->switchBranch(newBranch);
    }

    if (!switched)
        KMessageBoxHelper::error(this, i18n("Error switching to new target"));

    close();
}

#include "moc_switchbranchdialog.cpp"
