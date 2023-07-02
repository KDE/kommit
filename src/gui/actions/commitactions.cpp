/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitactions.h"
#include "dialogs/filestreedialog.h"
#include "dialogs/runnerdialog.h"
#include "diffwindow.h"

#include <QAction>

#include <KLocalizedString>

const QString &CommitActions::commitHash() const
{
    return mCommitHash;
}

void CommitActions::setCommitHash(const QString &newCommitHash)
{
    mCommitHash = newCommitHash;

    setActionEnabled(_actionBrowse, true);
    setActionEnabled(_actionCheckout, true);
    setActionEnabled(_actionDiff, true);
}

CommitActions::CommitActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionBrowse = addAction(i18n("Browse..."), this, &CommitActions::browse, false, true);
    _actionCheckout = addAction(i18n("Checkout..."), this, &CommitActions::checkout, false, true);
    _actionDiff = addAction(i18n("Diff with HEAD..."), this, &CommitActions::diff, false, true);
}

void CommitActions::browse()
{
    FilesTreeDialog d(mGit, mCommitHash, mParent);
    d.exec();
}

void CommitActions::checkout()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("checkout"), mCommitHash});
    d.exec();
}

void CommitActions::diff()
{
    auto d = new DiffWindow(mGit, mCommitHash, QStringLiteral("HEAD"));
    d->showModal();
}

#include "moc_commitactions.cpp"
