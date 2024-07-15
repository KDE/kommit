/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergedialog.h"

#include "caches/branchescache.h"
#include "commands/commandmerge.h"
#include "gitmanager.h"
#include "models/branchesmodel.h"

void MergeDialog::init(Git::Manager *git)
{
    comboBoxBranchName->addItems(git->branches()->names());
    labelToBranchName->setText(git->branches()->currentName());

    checkBoxAllowUnrelatedHistories->setCheckState(Qt::PartiallyChecked);
    checkBoxSquash->setCheckState(Qt::PartiallyChecked);

    stackedWidget->setCurrentWidget(pageEmpty);

    initComboBox<Git::CommandMerge::Strategy>(comboBoxStrategy);
    initComboBox<Git::CommandMerge::DiffAlgorithm>(comboBoxDiffAlgoritm);

    connect(comboBoxStrategy, &QComboBox::currentIndexChanged, this, &MergeDialog::slotComboBoxStrategyCurrentIndexChanged);
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
    cmd->setSquash(Git::checkStateToOptionalBool(checkBoxSquash->checkState()));

    switch (comboBoxFastForward->currentIndex()) {
    case 0:
        cmd->setFf(Git::FastForwardType::Unset);
        break;
    case 1:
        cmd->setFf(Git::FastForwardType::Yes);
        break;
    case 2:
        cmd->setFf(Git::FastForwardType::OnlyFastForward);
        break;
    case 3:
        cmd->setFf(Git::FastForwardType::No);
        break;
    }
    cmd->setStrategy(comboBoxCurrentValue<Git::CommandMerge::Strategy>(comboBoxStrategy));
    cmd->setIgnoreAllSpace(checkBoxIgnoreAllSpace->isChecked());
    cmd->setIgnoreSpaceAtEol(checkBoxIgnoreSpaceAtEol->isChecked());
    cmd->setIgnoreSpaceChange(checkBoxIgnoreSpaceChanges->isChecked());
    cmd->setIgnoreCrAtEol(checkBoxIgnoreCrAtEol->isChecked());
    cmd->setRenormalize(checkBoxRenormalize->isChecked());

    cmd->setOurs(radioButtonOurs->isChecked());
    cmd->setTheirs(radioButtonTheirs->isChecked());
    cmd->setFromBranch(comboBoxBranchName->currentText());
    cmd->setNoRenames(checkBoxNoRenames->isChecked());
    cmd->setDiffAlgorithm(comboBoxCurrentValue<Git::CommandMerge::DiffAlgorithm>(comboBoxDiffAlgoritm));

    return cmd;
}

void MergeDialog::slotComboBoxStrategyCurrentIndexChanged(int index)
{
    Q_UNUSED(index)

    auto strategy = comboBoxCurrentValue<Git::CommandMerge::Strategy>(comboBoxStrategy);

    switch (strategy) {
    case Git::CommandMerge::Ort:
        stackedWidget->setCurrentWidget(pageStrategyOrt);
        break;
    case Git::CommandMerge::Recursive:
        stackedWidget->setCurrentWidget(pageStrategyRecursive);
        break;
    default:
        stackedWidget->setCurrentWidget(pageEmpty);
        break;
    }
}

#include "moc_mergedialog.cpp"
