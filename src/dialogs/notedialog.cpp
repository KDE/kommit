/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "notedialog.h"

#include "git/gitmanager.h"

NoteDialog::NoteDialog(Git::Manager *git, const QString &branchName, QWidget *parent)
    : AppDialog(git, parent)
    , mBranchName{branchName}
{
    setupUi(this);

    textEdit->setText(git->readNote(branchName));
}

void NoteDialog::on_buttonBox_accepted()
{
    mGit->saveNote(mBranchName, textEdit->toPlainText());
    close();
}
