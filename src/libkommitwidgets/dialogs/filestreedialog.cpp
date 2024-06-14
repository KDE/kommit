/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filestreedialog.h"
#include "actions/fileactions.h"
#include "core/kmessageboxhelper.h"
#include "gitmanager.h"
#include "models/treemodel.h"

#include <entities/tree.h>

#include <KLocalizedString>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QMenu>
#include <interfaces.h>

FilesTreeDialog::FilesTreeDialog(Git::Manager *git, const QString &place, QWidget *parent)
    : AppDialog(git, parent)
    , mTreeModel(new TreeModel(this))
    , mPlace(place)
    , mActions(new FileActions(git, this))
    , mTreeViewMenu{new QMenu{this}}
{
    setupUi(this);

    // mActions->setPlace(place);

    mTreeModel->setSeparator(QStringLiteral("/"));

    const auto files = git->ls(place);

    mTreeModel->setShowRoot(true);
    mTreeModel->setLastPartAsData(true);
    QFileIconProvider p;
    mTreeModel->setDefaultIcon(p.icon(QFileIconProvider::Folder));
    mTreeModel->addData(files);
    treeView->setModel(mTreeModel);

    setWindowTitle(i18nc("@title:window", "Browse files: %1", place));

    lineEditBranchName->setText(place);

    listWidget->clear();

    const auto rootData = mTreeModel->rootData();
    for (const auto &f : rootData) {
        const QFileInfo fi(f);
        const auto icon = p.icon(fi);
        auto item = new QListWidgetItem(listWidget);
        item->setText(f);
        item->setIcon(icon);
        listWidget->addItem(item);
    }

    connect(treeView, &QTreeView::clicked, this, &FilesTreeDialog::slotTreeViewClicked);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &FilesTreeDialog::slotListWidgetCustomContextMenuRequested);
}

// FilesTreeDialog::FilesTreeDialog(Git::Manager *git, QSharedPointer<Git::Tree> tree, QWidget *parent)
//     : AppDialog(nullptr, parent)
//     , mTreeModel(new TreeModel(this))
//     , mPlace{}
//     , mActions(new FileActions(git, this))
//     , mTree{tree}
// {
//     setupUi(this);

//     auto files = tree->entries(Git::Tree::EntryType::File);

//     initModel(files);
// }

FilesTreeDialog::FilesTreeDialog(Git::Manager *git, QSharedPointer<Git::ITree> tree, QWidget *parent)
    : AppDialog(nullptr, parent)
    , mTreeModel(new TreeModel(this))
    , mPlace{}
    , mActions(new FileActions(git, this))
    , mTree{tree->tree()}
    , mTreeViewMenu{new QMenu{this}}
{
    setupUi(this);

    auto files = tree->tree()->entries(Git::Tree::EntryType::File);
    lineEditBranchName->setText(tree->treeTitle());
    setWindowTitle(i18nc("@title:window", "Browse files: %1", tree->treeTitle()));

    initModel(files);
}

void FilesTreeDialog::slotTreeViewCustomContextMenuRequested(const QPoint &pos)
{
    mExtractPerfix = mTreeModel->fullPath(treeView->currentIndex());

    mTreeViewMenu->popup(treeView->mapToGlobal(pos));
}

void FilesTreeDialog::slotTreeViewClicked(const QModelIndex &index)
{
    QFileIconProvider p;
    listWidget->clear();

    const auto d = mTreeModel->data(index);
    for (const auto &f : d) {
        const QFileInfo fi(f);
        const auto icon = p.icon(fi);
        auto item = new QListWidgetItem(listWidget);
        item->setText(f);
        item->setIcon(icon);
        listWidget->addItem(item);
    }
}

void FilesTreeDialog::slotExtract()
{
    auto path = QFileDialog::getExistingDirectory(this, i18n("Extract to"));
    if (path.isEmpty())
        return;
    auto ok = mTree->extract(path, mExtractPerfix);

    if (ok)
        KMessageBoxHelper::information(this, i18n("All file(s) extracted successfully"));
    else
        KMessageBoxHelper::error(this, i18n("An error occurred while extracting file(s)"));
}

void FilesTreeDialog::initModel(const QStringList &files)
{
    mTreeModel->setShowRoot(true);
    mTreeModel->setLastPartAsData(true);
    QFileIconProvider p;
    mTreeModel->setDefaultIcon(p.icon(QFileIconProvider::Folder));
    mTreeModel->addData(files);
    treeView->setModel(mTreeModel);

    const auto rootData = mTreeModel->rootData();
    for (const auto &f : rootData) {
        const QFileInfo fi(f);
        const auto icon = p.icon(fi);
        auto item = new QListWidgetItem(listWidget);
        item->setText(f);
        item->setIcon(icon);
        listWidget->addItem(item);
    }

    connect(treeView, &QTreeView::clicked, this, &FilesTreeDialog::slotTreeViewClicked);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &FilesTreeDialog::slotListWidgetCustomContextMenuRequested);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &FilesTreeDialog::slotTreeViewCustomContextMenuRequested);

    auto extractAction = mTreeViewMenu->addAction(i18n("Extract"));
    connect(extractAction, &QAction::triggered, this, &FilesTreeDialog::slotExtract);
}

void FilesTreeDialog::slotListWidgetCustomContextMenuRequested(const QPoint &pos)
{
    auto path = mTreeModel->fullPath(treeView->currentIndex());

    if (path == QLatin1Char('/'))
        path = listWidget->currentItem()->text();
    else
        path += QLatin1Char('/') + listWidget->currentItem()->text();

    auto file = mTree->file(path);

    mActions->setFile(file);
    mActions->popup(listWidget->mapToGlobal(pos));
}

#include "moc_filestreedialog.cpp"
