/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfilesdialog.h"
#include "GitKlientSettings.h"
#include "actions/changedfileactions.h"
#include "commitpushdialog.h"
#include "diffwindow.h"
#include "git/gitfile.h"
#include "git/gitmanager.h"

ChangedFilesDialog::ChangedFilesDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);
    reload();
    mActions = new ChangedFileActions(git, this);
    connect(mActions, &ChangedFileActions::reloadNeeded, this, &ChangedFilesDialog::reload);
}

void ChangedFilesDialog::on_pushButtonCommitPush_clicked()
{
    CommitPushDialog d(mGit, this);
    d.exec();
    reload();
}

void ChangedFilesDialog::reload()
{
    listWidget->clear();
    auto files = mGit->changedFiles();

    for (auto i = files.begin(); i != files.end(); ++i) {
        auto item = new QListWidgetItem(listWidget);
        item->setText(i.key());
        switch (i.value()) {
        case Git::Manager::Modified:
            item->setForeground(GitKlientSettings::diffModifiedColor());
            break;

        default:
            break;
        }
        listWidget->addItem(item);
    }
}

void ChangedFilesDialog::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    mActions->setFilePath(listWidget->currentItem()->text());
    mActions->diff();
}

void ChangedFilesDialog::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (listWidget->currentRow() == -1)
        return;

    mActions->setFilePath(listWidget->currentItem()->text());
    mActions->popup();
}
