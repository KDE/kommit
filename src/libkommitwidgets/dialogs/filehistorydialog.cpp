/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filehistorydialog.h"
#include "models/commitsmodel.h"

#include <caches/commitscache.h>
#include <entities/commit.h>
#include <repository.h>

#include <KLocalizedString>

FileHistoryDialog::FileHistoryDialog(Git::Repository *git, const QString &fileName, QWidget *parent)
    : AppDialog(git, parent)
    , mFileName(fileName)
{
    setupUi(this);

    // const auto hashes = git->fileLog(fileName);

    // const auto logs = git->commits();

    // for (const auto &hash : hashes) {
    //     auto log = logs->find(hash);
    //     if (log.isNull())
    //         continue;

    //     auto item = new QListWidgetItem(log.message());
    //     item->setData(dataRole, log.commitHash());
    //     listWidget->addItem(item);

    //     auto treeItem = new QTreeWidgetItem{treeWidget};
    //     treeItem->setText(0, log.message());
    //     treeItem->setData(0, dataRole, log.commitHash());
    //     treeWidget->addTopLevelItem(treeItem);
    // }
    load();
    plainTextEdit->setHighlighting(fileName);
    setWindowTitle(i18nc("@title:window", "File log: %1", fileName));

    connect(listWidget, &QListWidget::currentItemChanged, this, &FileHistoryDialog::slotListWidgetItemClicked);
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

FileHistoryDialog::FileHistoryDialog(Git::Repository *git, const Git::Blob &file, QWidget *parent)
    : FileHistoryDialog(git, file.name(), parent)
{
}

void FileHistoryDialog::load()
{
    auto commits = mGit->commits()->allCommits();
    for (Git::Commit &commit : commits) {
        auto file = commit.tree().file(mFileName);
        if (file.isNull())
            continue;
        for (auto &parentHash : commit.parents()) {
            auto parent = mGit->commits()->find(parentHash);
            if (parent.isNull())
                continue;

            auto parentFile = parent.tree().file(mFileName);

            if (file.content() != parentFile.content()) {
                // save the log

                auto item = new QListWidgetItem(commit.message());
                item->setData(dataRole, commit.commitHash());
                listWidget->addItem(item);

                auto treeItem = new QTreeWidgetItem{treeWidget};
                treeItem->setText(0, commit.message());
                treeItem->setData(0, dataRole, commit.commitHash());
                treeWidget->addTopLevelItem(treeItem);
            }
        }
    }
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

    const auto leftFileContent = mGit->fileContent(mLeftFile->data(0, dataRole).toString(), mFileName);
    const auto rightFileContent = mGit->fileContent(mRightFile->data(0, dataRole).toString(), mFileName);

    widgetDiffView->setOldFile(mFileName, leftFileContent);
    widgetDiffView->setNewFile(mFileName, rightFileContent);

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
