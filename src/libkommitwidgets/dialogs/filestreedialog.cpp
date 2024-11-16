/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filestreedialog.h"
#include "actions/fileactions.h"
#include "core/kmessageboxhelper.h"
#include "models/treemodel.h"
#include "repository.h"

#include <Kommit/ITree>
#include <entities/tree.h>

#include <KLocalizedString>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QMenu>

struct DirData : public NodeData {
    QStringList files;
};

FilesTreeDialog::FilesTreeDialog(Git::Repository *git, const QString &place, QWidget *parent)
    : AppDialog(git, parent)
    , mTreeModel(new TreeModel(this))
    , mPlace(place)
    , mActions(new FileActions(git, this))
    , mTreeViewMenu{new QMenu{this}}
{
    setupUi(this);

    // mActions->setPlace(place);

    mTreeModel->setSeparator(QStringLiteral("/"));
    mTreeModel->setShowRoot(true);
    mTreeModel->setRootTitle("/");

    Git::Tree tree{git->repoPtr(), place};
    initModel(tree);

    setWindowTitle(i18nc("@title:window", "Browse files: %1", place));

    lineEditBranchName->setText(place);
}

FilesTreeDialog::FilesTreeDialog(Git::Repository *git, Git::ITree *tree, QWidget *parent)
    : AppDialog(nullptr, parent)
    , mTreeModel(new TreeModel(this))
    , mPlace{}
    , mActions(new FileActions(git, this))
    , mTree{tree->tree()}
    , mTreeViewMenu{new QMenu{this}}
{
    setupUi(this);

    mTreeModel->setSeparator(QStringLiteral("/"));
    mTreeModel->setShowRoot(true);
    mTreeModel->setRootTitle("/");

    auto files = tree->tree().entries(Git::EntryType::File);
    lineEditBranchName->setText(tree->treeTitle());
    setWindowTitle(i18nc("@title:window", "Browse files: %1", tree->treeTitle()));

    initModel(tree->tree());
}

void FilesTreeDialog::slotTreeViewCustomContextMenuRequested(const QPoint &pos)
{
    mExtractPrefix = mTreeModel->fullPath(treeView->currentIndex());

    mTreeViewMenu->popup(treeView->mapToGlobal(pos));
}

void FilesTreeDialog::slotTreeViewClicked(const QModelIndex &index)
{
    QFileIconProvider p;
    listWidget->clear();

    const auto d = mTreeModel->node(index);
    for (auto &f : static_cast<DirData *>(d->nodeData)->files) {
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
    auto ok = mTree.extract(path, mExtractPrefix);

    if (ok)
        KMessageBoxHelper::information(this, i18n("All file(s) extracted successfully"));
    else
        KMessageBoxHelper::error(this, i18n("An error occurred while extracting file(s)"));
}

// void FilesTreeDialog::initModel(const QStringList &files)
// {
//     mTreeModel->setShowRoot(true);
//     mTreeModel->setLastPartAsData(true);
//     QFileIconProvider p;
//     mTreeModel->setDefaultIcon(p.icon(QFileIconProvider::Folder));

//     mTreeModel->addData(files);
//     treeView->setModel(mTreeModel);

//     const auto rootData = mTreeModel->rootData();
//     for (const auto &f : rootData) {
//         const QFileInfo fi(f);
//         const auto icon = p.icon(fi);
//         auto item = new QListWidgetItem(listWidget);
//         item->setText(f);
//         item->setIcon(icon);
//         listWidget->addItem(item);
//     }

//     connect(treeView, &QTreeView::clicked, this, &FilesTreeDialog::slotTreeViewClicked);
//     connect(listWidget, &QListWidget::customContextMenuRequested, this, &FilesTreeDialog::slotListWidgetCustomContextMenuRequested);
//     connect(treeView, &QTreeView::customContextMenuRequested, this, &FilesTreeDialog::slotTreeViewCustomContextMenuRequested);

//     auto extractAction = mTreeViewMenu->addAction(i18n("Extract"));
//     connect(extractAction, &QAction::triggered, this, &FilesTreeDialog::slotExtract);
// }

void FilesTreeDialog::initModel(const Git::Tree &tree)
{
    auto dirs = tree.entries(Git::EntryType::Dir);

    // mTreeModel->setShowRoot(true);
    // mTreeModel->setLastPartAsData(false);
    QFileIconProvider p;
    mTreeModel->setDefaultIcon(p.icon(QFileIconProvider::Folder));
    dirs.prepend("");

    for (auto const &dir : std::as_const(dirs)) {
        auto data = new DirData;
        data->files = tree.entries(dir, Git::EntryType::File);
        mTreeModel->addItem(dir, data);
    }

    treeView->setModel(mTreeModel);

    // const auto rootData = static_cast<DirData *>(mTreeModel->rootNode()->nodeData)->files;
    // for (const auto &f : rootData) {
    //     const QFileInfo fi(f);
    //     const auto icon = p.icon(fi);
    //     auto item = new QListWidgetItem(listWidget);
    //     item->setText(f);
    //     item->setIcon(icon);
    //     listWidget->addItem(item);
    // }

    connect(treeView, &QTreeView::clicked, this, &FilesTreeDialog::slotTreeViewClicked);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &FilesTreeDialog::slotListWidgetCustomContextMenuRequested);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &FilesTreeDialog::slotTreeViewCustomContextMenuRequested);

    auto extractAction = mTreeViewMenu->addAction(i18n("Extract"));
    connect(extractAction, &QAction::triggered, this, &FilesTreeDialog::slotExtract);
}

void FilesTreeDialog::slotListWidgetCustomContextMenuRequested(const QPoint &pos)
{
    auto node = mTreeModel->node(treeView->currentIndex());

    auto path = node->path + "/" + node->title;

    if (path == QLatin1Char('/'))
        path = listWidget->currentItem()->text();
    else
        path += QLatin1Char('/') + listWidget->currentItem()->text();

    auto file = mTree.file(path);

    mActions->setFile(file);
    mActions->popup(listWidget->mapToGlobal(pos));
}

#include "moc_filestreedialog.cpp"

#include <Kommit/ITree>
