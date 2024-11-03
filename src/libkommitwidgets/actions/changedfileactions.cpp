/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfileactions.h"
#include "caches/branchescache.h"
#include "repository.h"
#include "windows/diffwindow.h"

#include <Kommit/Blob>
#include <Kommit/Index>

#include <QAction>

#include <KLocalizedString>

ChangedFileActions::ChangedFileActions(Git::Repository *git, QWidget *parent)
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
    Git::Blob original;

    auto index = mGit->index();
    if (mOriginalFilePath == QString())
        original = index.blobByPath(mFilePath);
    else
        original = index.blobByPath(mOriginalFilePath);

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
