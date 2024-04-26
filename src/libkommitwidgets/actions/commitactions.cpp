/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitactions.h"
#include "dialogs/filestreedialog.h"
#include "dialogs/runnerdialog.h"
#include "windows/diffwindow.h"
#include <entities/commit.h>

#include <QAction>

#include <KLocalizedString>

QSharedPointer<Git::Commit> CommitActions::commit() const
{
    return mCommit;
}

void CommitActions::setCommit(QSharedPointer<Git::Commit> commit)
{
    mCommit = commit;

    setActionEnabled(_actionBrowse, !commit.isNull());
    setActionEnabled(_actionCheckout, !commit.isNull());
    setActionEnabled(_actionDiff, !commit.isNull());
}

CommitActions::CommitActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionBrowse = addAction(i18n("Browse"), this, &CommitActions::browse, false, true);
    _actionCheckout = addAction(i18n("Checkout"), this, &CommitActions::checkout, false, true);
    _actionDiff = addAction(i18n("Diff with working dir"), this, &CommitActions::diff, false, true);
}

void CommitActions::browse()
{
    FilesTreeDialog d(mGit, mCommit.dynamicCast<Git::ITree>(), mParent);
    d.exec();
}

void CommitActions::checkout()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("checkout"), mCommit->commitHash()});
    d.exec();
}

void CommitActions::diff()
{
    auto d = new DiffWindow(mGit, mCommit->tree());
    d->showModal();
}

#include "moc_commitactions.cpp"
