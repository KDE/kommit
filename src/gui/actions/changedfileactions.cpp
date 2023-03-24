/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfileactions.h"
#include "diffwindow.h"
#include "gitmanager.h"

#include <QAction>

#include <KLocalizedString>

ChangedFileActions::ChangedFileActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionDiff = addAction(i18n("Diff..."), this, &ChangedFileActions::diff, false, true);
    _actionRevert = addAction(i18n("Revert..."), this, &ChangedFileActions::revert, false, true);

    auto f = _actionDiff->font();
    f.setBold(true);
    _actionDiff->setFont(f);
}

const QString &ChangedFileActions::filePath() const
{
    return mFilePath;
}

void ChangedFileActions::setFilePath(const QString &newFilePath)
{
    mFilePath = newFilePath;
    mOriginalFilePath = QString();

    setActionEnabled(_actionDiff, true);
    setActionEnabled(_actionRevert, true);
}

void ChangedFileActions::setFilePaths(const QString &originalFilePath, const QString &renamedFilePath)
{
    mOriginalFilePath = originalFilePath;
    mFilePath = renamedFilePath;

    setActionEnabled(_actionDiff, true);
    setActionEnabled(_actionRevert, true);
}

void ChangedFileActions::diff()
{
    Git::File original;

    if (mOriginalFilePath == QString())
        original = Git::File{mGit, mGit->currentBranch(), mFilePath};
    else
        original = Git::File{mGit, mGit->currentBranch(), mOriginalFilePath};

    const Git::File changed{mGit->path() + QLatin1Char('/') + mFilePath};

    auto diffWin = new DiffWindow(original, changed);
    diffWin->showModal();
}

void ChangedFileActions::revert()
{
    mGit->revertFile(mFilePath);
    Q_EMIT reloadNeeded();
}
