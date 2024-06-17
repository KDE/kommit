/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filehistorydialog.h"
#include "models/logsmodel.h"

#include <entities/commit.h>
#include <gitmanager.h>

#include <KLocalizedString>

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
        item->setData(dataRole, log->commitHash());
        listWidget->addItem(item);

        auto treeItem = new QTreeWidgetItem{treeWidget};
        treeItem->setText(0, log->subject());
        treeItem->setData(0, dataRole, log->commitHash());
        treeWidget->addTopLevelItem(treeItem);
    }
    plainTextEdit->setHighlighting(fileName);
    setWindowTitle(i18nc("@title:window", "File log: %1", fileName));

    connect(listWidget, &QListWidget::itemClicked, this, &FileHistoryDialog::slotListWidgetItemClicked);
    connect(treeWidget, &QTreeWidget::itemClicked, this, &FileHistoryDialog::slotTreeViewItemClicked);
    connect(radioButtonRegularView, &QRadioButton::toggled, this, &FileHistoryDialog::slotRadioButtonRegularViewToggled);
    connect(radioButtonDifferentialView, &QRadioButton::toggled, this, &FileHistoryDialog::slotRadioButtonDifferentialViewToggled);
    widgetDiffView->showSameSize(true);

    treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    treeWidget->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    treeWidget->header()->setSectionResizeMode(2, QHeaderView::Fixed);
    treeWidget->header()->setStretchLastSection(false);
    radioButtonRegularView->setChecked(true);
}

FileHistoryDialog::FileHistoryDialog(Git::Manager *git, QSharedPointer<Git::File> file, QWidget *parent)
    : FileHistoryDialog(git, file->fileName(), parent)
{
}

void FileHistoryDialog::slotListWidgetItemClicked(QListWidgetItem *item)
{
    const auto content = mGit->fileContent(item->data(dataRole).toString(), mFileName);
    plainTextEdit->setPlainText(content);
}

void FileHistoryDialog::slotRadioButtonRegularViewToggled(bool toggle)
{
    if (toggle) {
        stackedWidgetFileSelector->setCurrentIndex(0);
        stackedWidgetFileView->setCurrentIndex(0);
    }
}

void FileHistoryDialog::slotRadioButtonDifferentialViewToggled(bool toggle)
{
    if (toggle) {
        stackedWidgetFileSelector->setCurrentIndex(1);
        stackedWidgetFileView->setCurrentIndex(1);
    }
}

void FileHistoryDialog::compareFiles()
{
    if (!mLeftFile || !mRightFile)
        return;

    widgetDiffView->setOldFile(QSharedPointer<Git::File>{new Git::File{mGit, mLeftFile->data(0, dataRole).toString(), mFileName}});
    widgetDiffView->setNewFile(QSharedPointer<Git::File>{new Git::File{mGit, mRightFile->data(0, dataRole).toString(), mFileName}});
    widgetDiffView->compare();
}

void FileHistoryDialog::slotTreeViewItemClicked(QTreeWidgetItem *item, int column)
{
    if (column == 1) {
        item->setText(1, QStringLiteral("✓"));
        if (mLeftFile)
            mLeftFile->setText(1, QLatin1String());
        mLeftFile = item;
    }
    if (column == 2) {
        item->setText(2, QStringLiteral("✓"));
        if (mRightFile)
            mRightFile->setText(2, QLatin1String());
        mRightFile = item;
    }
    compareFiles();
}

#include "moc_filehistorydialog.cpp"
