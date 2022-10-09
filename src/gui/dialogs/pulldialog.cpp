/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "pulldialog.h"
#include "runnerdialog.h"

#include "commands/commandpull.h"
#include "gitmanager.h"
#include "models/branchesmodel.h"
#include "models/remotesmodel.h"
#include <QDialogButtonBox>

PullDialog::PullDialog(QWidget *parent, Git::Manager *git)
    : AppDialog(parent)
{
    setupUi(this);

    auto g = git;
    if (!g)
        g = Git::Manager::instance();

    comboBoxRemote->addItems(g->remotes());
    comboBoxBranch->addItems(g->branches());

    comboBoxBranch->setCurrentText(g->currentBranch());
    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &PullDialog::slotAccepted);
}

void PullDialog::slotAccepted()
{
    Git::CommandPull cmd;

    cmd.setRemote(comboBoxRemote->currentText());
    cmd.setBranch(comboBoxBranch->currentText());
    cmd.setSquash(checkBoxSquash->isChecked());
    cmd.setNoFf(checkBoxNoFastForward->isChecked());
    cmd.setFfOnly(checkBoxFastForwardOnly->isChecked());
    cmd.setNoCommit(checkBoxNoCommit->isChecked());
    cmd.setPrune(checkBoxPrune->isChecked());
    cmd.setTags(checkBoxTags->isChecked());

    RunnerDialog d(this);
    d.run(&cmd);
    d.exec();

    accept();
}
