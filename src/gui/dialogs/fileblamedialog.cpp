/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileblamedialog.h"

#include "gitmanager.h"
#include "models/logsmodel.h"
#include "qdebug.h"

#include <KLocalizedString>
#include <QThread>

FileBlameDialog::FileBlameDialog(Git::Manager *git, const Git::File &file, QWidget *parent)
    : AppDialog(git, parent)
    , mFile(file)
{
    setupUi(this);

    logDetailsWidget->setEnableCommitsLinks(false);
    plainTextEdit->setShowTitleBar(false);

    connect(plainTextEdit, &BlameCodeView::blockSelected, this, &FileBlameDialog::slotPlainTextEditBlockSelected);

    if (git->logsModel()->isLoaded())
        loadData();
    else
        connect(mGit->logsModel(), &Git::LogsModel::loaded, this, &FileBlameDialog::loadData);
}

void FileBlameDialog::loadData()
{
    plainTextEdit->setHighlighting(mFile.fileName());

    const auto b = mGit->blame(mFile);
    plainTextEdit->setBlameData(b);

    setWindowTitle(i18nc("@title:window", "Blame file: %1", mFile.fileName()));
}

void FileBlameDialog::slotPlainTextEditBlockSelected()
{
    auto data = plainTextEdit->currentBlockData();
    auto log = mGit->logsModel()->findLogByHash("05659b9f92b7932bb2c04ced181dbdde294cb0b");
    qDebug() << log;
    logDetailsWidget->setLog(data ? static_cast<Git::Log *>(data->data) : nullptr);
}
