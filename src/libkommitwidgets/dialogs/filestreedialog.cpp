/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filestreedialog.h"
#include "actions/fileactions.h"
#include "gitmanager.h"
#include "models/treemodel.h"

#include <entities/tree.h>

#include <KLocalizedString>
#include <QFileIconProvider>

FilesTreeDialog::FilesTreeDialog(Git::Manager *git, const QString &place, QWidget *parent)
    : AppDialog(git, parent)
    , mTreeModel(new TreeModel(this))
    , mPlace(place)
    , mActions(new FileActions(git, this))
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

FilesTreeDialog::FilesTreeDialog(Git::Manager *git, QSharedPointer<Git::Tree> tree, QWidget *parent)
    : AppDialog(nullptr, parent)
    , mTreeModel(new TreeModel(this))
    , mPlace{}
    , mActions(new FileActions(git, this))
    , mTree{tree}
{
    setupUi(this);

    auto files = tree->entries(Git::Tree::EntryType::File);

    initModel(files);
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
#include <entities/tree.h>
