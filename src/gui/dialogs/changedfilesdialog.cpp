/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfilesdialog.h"
#include "GitKlientSettings.h"
#include "actions/changedfileactions.h"
#include "commitpushdialog.h"
#include "gitmanager.h"

#include <KLocalizedString>

ChangedFilesDialog::ChangedFilesDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
    , mActions(new ChangedFileActions(git, this))
{
    setupUi(this);
    reload();
    connect(mActions, &ChangedFileActions::reloadNeeded, this, &ChangedFilesDialog::reload);
    buttonBox->button(QDialogButtonBox::Ok)->setText(i18n("Commit/Push"));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ChangedFilesDialog::slotPushCommit);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ChangedFilesDialog::reject);
    connect(buttonBox, &QDialogButtonBox::clicked, this, &ChangedFilesDialog::slotButtonBoxClicked);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &ChangedFilesDialog::slotItemDoubleClicked);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &ChangedFilesDialog::slotCustomContextMenuRequested);

    buttonBox->button(QDialogButtonBox::RestoreDefaults)->setText(i18n("Reload"));
}

void ChangedFilesDialog::slotPushCommit()
{
    CommitPushDialog d(mGit, this);
    d.exec();
    reload();
}

void ChangedFilesDialog::slotButtonBoxClicked(QAbstractButton *button)
{
    if (button == buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
        reload();
    }
}

void ChangedFilesDialog::reload()
{
    listWidget->clear();
    auto files = mGit->changedFiles();

    if (!files.size()) {
        auto item = new QListWidgetItem(listWidget);
        item->setText(i18n("You don't have any modified file!"));
        listWidget->addItem(item);
        listWidget->setDisabled(true);
        return;
    }
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

void ChangedFilesDialog::slotItemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    mActions->setFilePath(listWidget->currentItem()->text());
    mActions->diff();
}

void ChangedFilesDialog::slotCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (listWidget->currentRow() == -1)
        return;

    mActions->setFilePath(listWidget->currentItem()->text());
    mActions->popup();
}
