/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfilesdialog.h"
#include "actions/changedfileactions.h"
#include "commitpushdialog.h"
#include "gitmanager.h"
#include "models/changedfilesmodel.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <KWindowConfig>

#include <QInputDialog>
#include <QWindow>

namespace
{
static const char myChangedFilesDialogGroupName[] = "ChangedFilesDialog";
}
ChangedFilesDialog::ChangedFilesDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
    , mActions(new ChangedFileActions(git, this))
    , mModel(new ChangedFilesModel(git, false, this))
{
    setupUi(this);

    connect(mActions, &ChangedFileActions::reloadNeeded, mModel, &ChangedFilesModel::reload);

    connect(pushButtonCommitPush, &QPushButton::clicked, this, &ChangedFilesDialog::slotPushCommit);
    connect(pushButtonReload, &QPushButton::clicked, mModel, &ChangedFilesModel::reload);
    connect(pushButtonStashChanges, &QPushButton::clicked, this, &ChangedFilesDialog::slotStash);
    connect(listView, &QListView::doubleClicked, this, &ChangedFilesDialog::slotItemDoubleClicked);
    connect(listView, &QListView::customContextMenuRequested, this, &ChangedFilesDialog::slotCustomContextMenuRequested);

    listView->setModel(mModel);
    mModel->reload();
    readConfig();
}

ChangedFilesDialog::~ChangedFilesDialog()
{
    writeConfig();
}

void ChangedFilesDialog::slotPushCommit()
{
    CommitPushDialog d(mGit, this);
    d.exec();
    mModel->reload();
}

void ChangedFilesDialog::slotStash()
{
    if (mGit->changedFiles().empty()) {
        KMessageBox::information(this, i18n("You don't have any changes!"), i18n("Stash"));
        return;
    }
    bool ok;

    const auto name = QInputDialog::getText(this, i18n("Create new stash"), i18n("Name of stash"), QLineEdit::Normal, QString(), &ok);

    if (ok) {
        mGit->createStash(name);
        mModel->reload();
    }
}

void ChangedFilesDialog::slotItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto data = mModel->data(index.row());
    if (data->oldFilePath != QString())
        mActions->setFilePaths(data->oldFilePath, data->filePath);
    else
        mActions->setFilePath(data->filePath);
    mActions->diff();
}

void ChangedFilesDialog::slotCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (!listView->currentIndex().isValid())
        return;

    mActions->setFilePath(mModel->filePath(listView->currentIndex().row()));
    mActions->popup();
}

void ChangedFilesDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(800, 300));
    KConfigGroup group(KSharedConfig::openStateConfig(), myChangedFilesDialogGroupName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void ChangedFilesDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myChangedFilesDialogGroupName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
}

#include "moc_changedfilesdialog.cpp"
