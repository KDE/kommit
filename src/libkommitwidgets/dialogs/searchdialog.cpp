/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "searchdialog.h"

#include <Kommit/BranchesCache>
#include <Kommit/Commit>
#include <Kommit/CommitsCache>
#include <Kommit/File>
#include <Kommit/Repository>
#include <Kommit/Tree>

#include <KLocalizedString>

#include <QStandardItemModel>
#include <QtConcurrent>

#include "fileviewerdialog.h"

SearchDialog::SearchDialog(const QString &path, Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
    , mModel(new QStandardItemModel(this))
{
    setupUi(this);
    treeView->setModel(mModel);

    initModel();
    lineEditPath->setText(path);

    connect(pushButtonSearch, &QPushButton::clicked, this, &SearchDialog::slotPushButtonSearchClicked);
    connect(treeView, &QTreeView::doubleClicked, this, &SearchDialog::slotTreeViewDoubleClicked);
}

void SearchDialog::initModel()
{
    mModel->setColumnCount(2);
    mModel->setHeaderData(0, Qt::Horizontal, i18n("File name"));
    mModel->setHeaderData(1, Qt::Horizontal, i18n("Place"));
}

SearchDialog::SearchDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
    , mModel(new QStandardItemModel(this))
{
    setupUi(this);
    treeView->setModel(mModel);
    initModel();

    connect(pushButtonSearch, &QPushButton::clicked, this, &SearchDialog::slotPushButtonSearchClicked);
    connect(treeView, &QTreeView::doubleClicked, this, &SearchDialog::slotTreeViewDoubleClicked);
}

void SearchDialog::slotPushButtonSearchClicked()
{
    mModel->clear();
    initModel();
    mTimerId = startTimer(500);
    pushButtonSearch->setEnabled(false);
    mProgress.isCommit = radioButtonSearchCommits->isChecked();
    mProgress.total = mProgress.value = 0;
    QThreadPool::globalInstance()->start([this]() {
        beginSearch();
    });
}

void SearchDialog::slotTreeViewDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    auto fileNameIndex = mModel->index(index.row(), 0);
    auto placeIndex = mModel->index(index.row(), 1);
    const auto fileName = mModel->data(fileNameIndex).toString();
    const auto place = mModel->data(placeIndex).toString();

    Git::Tree tree;

    if (mProgress.isCommit)
        tree = mGit->commits()->find(place).tree();
    else
        tree = mGit->branches()->findByName(place).tree();

    auto file = tree.file(fileName);
    qDebug() << fileName << place << mProgress.isCommit << file.name();

    auto d = new FileViewerDialog(file);
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->show();
}

void SearchDialog::beginSearch()
{
    if (radioButtonSearchBranches->isChecked()) {
        auto branches = mGit->branches()->allBranches(Git::BranchType::LocalBranch);

        mProgress.total = branches.size();
        for (Git::Branch &branch : branches) {
            mProgress.currentPlace = branch.name();
            mProgress.value++;
            searchOnTree(branch.tree(), branch.name());
        };
    } else {
        auto commits = mGit->commits()->allCommits();
        mProgress.total = commits.size();
        for (Git::Commit &commit : commits) {
            mProgress.currentPlace = commit.message();
            mProgress.value++;
            searchOnTree(commit.tree(), commit.commitHash());
        }
    }

    pushButtonSearch->setEnabled(true);

    metaObject()->invokeMethod(this, "searchFinished");
}

void SearchDialog::searchOnTree(const Git::Tree &tree, const QString &place)
{
    auto files = tree.entries(Git::EntryType::File);
    auto searchPath = lineEditPath->text();
    auto search = lineEditText->text().toUtf8();

    for (auto &fileName : files) {
        if (!searchPath.isEmpty() && !searchPath.contains(fileName, Qt::CaseInsensitive))
            continue;
        auto file = tree.file(fileName);

        if (file.isNull() || file.isBinary())
            continue;
        if (file.content().contains(search)) {
            mResultsLock.lockForWrite();
            mProgress.found++;
            mProgress.foundFiles << SearchResult{file, place};
            mResultsLock.unlock();
        }
    }
}

void SearchDialog::addSearchResult(const Git::Blob &file)
{
    // qDebug() << file.name();
    mModel->appendRow(new QStandardItem(file.name()));
}

void SearchDialog::searchFinished()
{
    killTimer(mTimerId);
    for (auto &f : std::as_const(mProgress.foundFiles)) {
        // auto nameItem = new QStandardItem(file.name());
        // addSearchResult(f.file);
        mModel->appendRow({new QStandardItem(f.file.filePath()), new QStandardItem(f.place)});
    }

    mProgress.foundFiles.clear();
}

void SearchDialog::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    progressBar->setMaximum(mProgress.total);
    progressBar->setValue(mProgress.value);

    // mResultsLock.lockForWrite();
    // for (auto f : std::as_const(mProgress.foundFiles))
    //     addSearchResult(f);
    // mProgress.foundFiles.clear();
    // mResultsLock.unlock();
}

#include "moc_searchdialog.cpp"
