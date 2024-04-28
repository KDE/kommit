/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "selectbranchestodiffdialog.h"
#include "core/kmessageboxhelper.h"

#include "gitmanager.h"
#include <KLocalizedString>

SelectBranchesToDiffDialog::SelectBranchesToDiffDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);

    const auto branches = git->branchesNames(Git::Manager::BranchType::LocalBranch);

    comboBoxOldBranch->addItems(branches);
    comboBoxNewBranch->addItems(branches);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SelectBranchesToDiffDialog::slotAccepted);
}

QString SelectBranchesToDiffDialog::oldBranch() const
{
    if (radioButtonLeftIsHead->isChecked())
        return QStringLiteral("HEAD");
    return comboBoxOldBranch->currentText();
}

QString SelectBranchesToDiffDialog::newBranch() const
{
    if (radioButtonRightIsHead->isChecked())
        return QStringLiteral("HEAD");
    return comboBoxNewBranch->currentText();
}

void SelectBranchesToDiffDialog::slotAccepted()
{
    if (oldBranch() == newBranch()) {
        KMessageBoxHelper::error(this, i18n("The selected branches must be different!"), i18n("Select Branches"));
        return;
    }
    accept();
}

#include "moc_selectbranchestodiffdialog.cpp"
