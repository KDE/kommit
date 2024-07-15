/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileblamedialog.h"

#include "gitmanager.h"
#include "models/commitsmodel.h"

#include <KLocalizedString>
#include <QThread>

FileBlameDialog::FileBlameDialog(Git::Manager *git, QSharedPointer<Git::File> file, QWidget *parent)
    : AppDialog(git, parent)
    , mFile(file)
{
    setupUi(this);

    widgetCommitDetails->setEnableCommitsLinks(false);
    plainTextEdit->setShowTitleBar(false);

    connect(plainTextEdit, &BlameCodeView::blockSelected, this, &FileBlameDialog::slotPlainTextEditBlockSelected);

    // if (git->commitsCache()->isLoaded())
    loadData();
    // else
    //     connect(mGit->logsModel(), &Git::LogsModel::loaded, this, &FileBlameDialog::loadData);
}

void FileBlameDialog::loadData()
{
    plainTextEdit->setHighlighting(mFile->fileName());

    const auto b = mGit->blame(mFile);
    plainTextEdit->setBlameData(b);

    setWindowTitle(i18nc("@title:window", "Blame file: %1", mFile->fileName()));
}

void FileBlameDialog::slotPlainTextEditBlockSelected()
{
    auto data = plainTextEdit->currentBlockData();
    widgetCommitDetails->setCommit(data ? static_cast<Git::Commit *>(data->data) : nullptr);
}

#include "moc_fileblamedialog.cpp"
