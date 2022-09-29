/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reposettingsdialog.h"

#include "git/gitmanager.h"

RepoSettingsDialog::RepoSettingsDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(parent)
    , mGit(git)
{
    setupUi(this);

    lineEditUserName->setText(git->config(QStringLiteral("user.name")));
    lineEditUserEmail->setText(git->config(QStringLiteral("user.email")));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &RepoSettingsDialog::slotAccepted);
}

void RepoSettingsDialog::slotAccepted()
{
    mGit->setConfig(QStringLiteral("user.name"), lineEditUserName->text());
    mGit->setConfig(QStringLiteral("user.email"), lineEditUserEmail->text());
    accept();
}
