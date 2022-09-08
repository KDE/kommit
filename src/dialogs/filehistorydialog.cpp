/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filehistorydialog.h"

#include "git/gitmanager.h"
#include "git/gitlog.h"
#include "git/models/logsmodel.h"

#include <KLocalizedString>

FileHistoryDialog::FileHistoryDialog(QWidget *parent) :
      AppDialog(parent)
{
    setupUi(this);
    _git = Git::Manager::instance();
}

FileHistoryDialog::FileHistoryDialog(Git::Manager *git, const QString &fileName, QWidget *parent):
      AppDialog(parent), _git(git), _fileName(fileName)
{
    setupUi(this);

    auto hashes = git->fileLog(fileName);

    auto logs = git->logsModel();


    for (auto &hash: hashes) {
        auto log = logs->findLogByHash(hash);
        if (!log)
            continue;

        auto item = new QListWidgetItem(log->subject());
        item->setData(Qt::UserRole + 1, log->commitHash());
        listWidget->addItem(item);
    }
    plainTextEdit->setHighlighting(fileName);
    setWindowTitle(i18nc("@title:window", "File log: %1", fileName));
}

FileHistoryDialog::FileHistoryDialog(Git::Manager *git, const Git::File &file, QWidget *parent)
    : AppDialog(parent), _git(git)
{
    setupUi(this);

    _fileName = file.fileName();
    const auto hashes = git->fileLog(file.fileName());

    auto logs = git->logsModel();


    for (const auto &hash: hashes) {
        auto log = logs->findLogByHash(hash);
        if (!log)
            continue;

        auto item = new QListWidgetItem(log->subject());
        item->setData(Qt::UserRole + 1, log->commitHash());
        listWidget->addItem(item);
    }
    plainTextEdit->setHighlighting(file.fileName());
    setWindowTitle(i18nc("@title:window", "File log: %1", file.fileName()));
}

void FileHistoryDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
    auto content = _git->fileContent(item->data(Qt::UserRole + 1).toString(), _fileName);
    plainTextEdit->setPlainText(content);
}
