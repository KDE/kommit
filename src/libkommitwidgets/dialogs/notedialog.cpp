/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "notedialog.h"

#include <entities/commit.h>
#include <entities/note.h>
#include <gitmanager.h>

#include <QPushButton>

NoteDialog::NoteDialog(Git::Manager *git, const QString &branchName, QWidget *parent)
    : AppDialog(git, parent)
    , mBranchName{branchName}
{
    setupUi(this);

    textEdit->setText(git->readNote(branchName));
    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &NoteDialog::slotAccepted);
}

NoteDialog::NoteDialog(Git::Manager *git, QSharedPointer<Git::Commit> commit, QWidget *parent)
    : AppDialog(git, parent)
    , mCommit{commit}
{
    setupUi(this);

    auto note = commit->note();

    if (!note.isNull())
        textEdit->setText(note->mesage());

    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &NoteDialog::slotAccepted);
}

void NoteDialog::slotAccepted()
{
    if (mCommit.isNull())
        mGit->saveNote(mBranchName, textEdit->toPlainText());
    else
        mCommit->createNote(textEdit->toPlainText());
    accept();
}

#include "moc_notedialog.cpp"
