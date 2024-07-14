/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchdialog.h"

#include "caches/branchescache.h"
#include "caches/remotescache.h"
#include "commands/commandfetch.h"
#include "gitmanager.h"
#include "observers/fetchobserver.h"
#include "runnerdialog.h"

#include <QDialogButtonBox>
FetchDialog::FetchDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
    , mObserver{new Git::FetchObserver{git}}
{
    setupUi(this);

    comboBoxRemote->addItems(git->remotes()->allNames());
    comboBoxBranch->addItems(git->branches()->names(Git::BranchType::LocalBranch));

    comboBoxRemote->setCurrentText(git->branches()->currentName());
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FetchDialog::slotAccept);
}

void FetchDialog::setBranch(const QString &branch)
{
    comboBoxBranch->setCurrentText(branch);
}

void FetchDialog::slotAccept()
{
    //    mGit->fetch(comboBoxRemote->currentText(), mObserver);
    Git::CommandFetch *cmd = new Git::CommandFetch;

    cmd->setRemote(comboBoxRemote->currentText());

    if (!checkBoxAllBranches->isChecked())
        cmd->setBranch(comboBoxBranch->currentText());
    cmd->setNoFf(checkBoxNoFastForward->isChecked());
    cmd->setFfOnly(checkBoxFastForwardOnly->isChecked());
    cmd->setNoCommit(checkBoxNoCommit->isChecked());
    cmd->setPrune(checkBoxPrune->isChecked());
    cmd->setTags(checkBoxTags->isChecked());

    RunnerDialog d(mGit, this);
    d.run(cmd);
    d.exec();

    accept();
}

#include "moc_fetchdialog.cpp"
