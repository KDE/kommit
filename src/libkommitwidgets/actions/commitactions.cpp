/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitactions.h"
#include "dialogs/filestreedialog.h"
#include "dialogs/notedialog.h"
#include "dialogs/runnerdialog.h"
#include "windows/diffwindow.h"
#include <entities/commit.h>

#include <QAction>

#include <KLocalizedString>

const Git::Commit &CommitActions::commit() const
{
    return mCommit;
}

void CommitActions::setCommit(const Git::Commit &commit)
{
    mCommit = commit;

    setActionEnabled(_actionBrowse, !commit.isNull());
    setActionEnabled(_actionCheckout, !commit.isNull());
    setActionEnabled(_actionDiff, !commit.isNull());
    setActionEnabled(_actionNote, !commit.isNull());
}

CommitActions::CommitActions(Git::Repository *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionBrowse = addAction(i18nc("@action", "Browse"), this, &CommitActions::browse, false, true);
    _actionCheckout = addAction(i18nc("@action", "Checkout"), this, &CommitActions::checkout, false, true);
    _actionDiff = addAction(i18nc("@action", "Diff with working dir"), this, &CommitActions::diff, false, true);
    _actionNote = addAction(i18nc("@action", "Note"), this, &CommitActions::note, false, true);
}

void CommitActions::browse()
{
    FilesTreeDialog d(mGit, &mCommit, mParent);
    d.exec();
}

void CommitActions::checkout()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("checkout"), mCommit.commitHash()});
    d.exec();
}

void CommitActions::diff()
{
    auto d = new DiffWindow(mGit, &mCommit);
    d->showModal();
}
void CommitActions::note()
{
    NoteDialog d{mGit, mCommit, mParent};
    d.exec();
}

#include "moc_commitactions.cpp"
