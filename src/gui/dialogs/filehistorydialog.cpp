/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filehistorydialog.h"

#include "gitlog.h"
#include "gitmanager.h"
#include "models/logsmodel.h"

#include <KLocalizedString>

// TODO: remove ctor without git input
FileHistoryDialog::FileHistoryDialog(QWidget *parent)
    : AppDialog(Git::Manager::instance(), parent)
{
    setupUi(this);

    connect(listWidget, &QListWidget::itemClicked, this, &FileHistoryDialog::slotListWidgetItemClicked);
}

FileHistoryDialog::FileHistoryDialog(Git::Manager *git, const QString &fileName, QWidget *parent)
    : AppDialog(git, parent)
    , mFileName(fileName)
{
    setupUi(this);

    const auto hashes = git->fileLog(fileName);

    const auto logs = git->logsModel();

    for (const auto &hash : hashes) {
        auto log = logs->findLogByHash(hash);
        if (!log)
            continue;

        auto item = new QListWidgetItem(log->subject());
        item->setData(Qt::UserRole + 1, log->commitHash());
        listWidget->addItem(item);
    }
    plainTextEdit->setHighlighting(fileName);
    setWindowTitle(i18nc("@title:window", "File log: %1", fileName));

    connect(listWidget, &QListWidget::itemClicked, this, &FileHistoryDialog::slotListWidgetItemClicked);
}

FileHistoryDialog::FileHistoryDialog(Git::Manager *git, const Git::File &file, QWidget *parent)
    : FileHistoryDialog(git, file.fileName(), parent)
{
}

void FileHistoryDialog::slotListWidgetItemClicked(QListWidgetItem *item)
{
    const auto content = mGit->fileContent(item->data(Qt::UserRole + 1).toString(), mFileName);
    plainTextEdit->setPlainText(content);
}
