/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfileactions.h"
#include "caches/branchescache.h"
#include "gitmanager.h"
#include "windows/diffwindow.h"
#include <entities/blob.h>

#include <QAction>

#include <KLocalizedString>

ChangedFileActions::ChangedFileActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionDiff = addAction(i18nc("@action", "Diff…"), this, &ChangedFileActions::diff, false, true);
    _actionRevert = addAction(i18nc("@action", "Revert…"), this, &ChangedFileActions::revert, false, true);

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
    QSharedPointer<Git::Blob> original;

    if (mOriginalFilePath == QString())
        original.reset(new Git::Blob{mGit->repoPtr(), mFilePath});
    else
        original.reset(new Git::Blob{mGit->repoPtr(), mOriginalFilePath});

    QSharedPointer<Git::Blob> changed{new Git::Blob{mGit->repoPtr(), mFilePath}};

    auto diffWin = new DiffWindow;
    diffWin->setLeft(original);
    diffWin->setRight(mGit->path() + mFilePath);
    diffWin->compare();
    diffWin->showModal();
}

void ChangedFileActions::revert()
{
    mGit->revertFile(mFilePath);
    Q_EMIT reloadNeeded();
}

#include "moc_changedfileactions.cpp"
