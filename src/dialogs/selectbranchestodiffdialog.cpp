/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "selectbranchestodiffdialog.h"

#include "git/gitmanager.h"
#include <KLocalizedString>
#include <KMessageBox>

SelectBranchesToDiffDialog::SelectBranchesToDiffDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);

    auto branches = git->branches();

    comboBoxOldBranch->addItems(branches);
    comboBoxNewBranch->addItems(branches);
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

void SelectBranchesToDiffDialog::on_buttonBox_accepted()
{
    if (oldBranch() == newBranch()) {
        KMessageBox::error(this, i18n("The selected branches must be different!"));
        return;
    }
    accept();
}
