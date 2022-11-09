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

ChangedFilesDialog::ChangedFilesDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
    , mActions(new ChangedFileActions(git, this))
    , mModel(new ChangedFilesModel(git, false, this))
{
    setupUi(this);

    connect(mActions, &ChangedFileActions::reloadNeeded, mModel, &ChangedFilesModel::reload);
    buttonBox->button(QDialogButtonBox::Ok)->setText(i18n("Commit/Push"));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ChangedFilesDialog::slotPushCommit);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ChangedFilesDialog::reject);
    connect(buttonBox, &QDialogButtonBox::clicked, this, &ChangedFilesDialog::slotButtonBoxClicked);
    connect(listView, &QListView::doubleClicked, this, &ChangedFilesDialog::slotItemDoubleClicked);
    connect(listView, &QListView::customContextMenuRequested, this, &ChangedFilesDialog::slotCustomContextMenuRequested);

    buttonBox->button(QDialogButtonBox::RestoreDefaults)->setText(i18n("Reload"));

    listView->setModel(mModel);
    mModel->reload();
}

void ChangedFilesDialog::slotPushCommit()
{
    CommitPushDialog d(mGit, this);
    d.exec();
    mModel->reload();
}

void ChangedFilesDialog::slotButtonBoxClicked(QAbstractButton *button)
{
    if (button == buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
        mModel->reload();
    }
}

void ChangedFilesDialog::slotItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    mActions->setFilePath(mModel->filePath(index.row()));
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
