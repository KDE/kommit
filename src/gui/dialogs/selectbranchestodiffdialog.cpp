/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "selectbranchestodiffdialog.h"

#include "gitmanager.h"
#include <KLocalizedString>
#include <KMessageBox>

SelectBranchesToDiffDialog::SelectBranchesToDiffDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);

    const auto branches = git->branches();

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
        KMessageBox::error(this, i18n("The selected branches must be different!"), i18n("Select Branches"));
        return;
    }
    accept();
}

#include "moc_selectbranchestodiffdialog.cpp"
