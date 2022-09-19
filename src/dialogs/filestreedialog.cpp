/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filestreedialog.h"
#include "actions/fileactions.h"
#include "dialogs/filehistorydialog.h"
#include "dialogs/fileviewerdialog.h"
#include "dialogs/searchdialog.h"
#include "git/gitmanager.h"
#include "models/treemodel.h"
#include <QFileIconProvider>

FilesTreeDialog::FilesTreeDialog(const QString &place, QWidget *parent)
    : AppDialog(parent)
    , mTreeModel(new TreeModel(this))
    , mPlace(place)
{
    setupUi(this);

    mActions = new FileActions(Git::Manager::instance(), this);
    mActions->setPlace(place);

    mTreeModel->setSeparator(QStringLiteral("/"));

    auto files = Git::Manager::instance()->ls(place);

    mTreeModel->setLastPartAsData(true);
    QFileIconProvider p;
    mTreeModel->setDefaultIcon(p.icon(QFileIconProvider::Folder));
    mTreeModel->addData(files);
    treeView->setModel(mTreeModel);

    setWindowTitle(i18nc("@title:window", "Browse files: %1", place));

    lineEditBranchName->setText(place);

    listWidget->clear();

    for (const auto &f : mTreeModel->rootData()) {
        QFileInfo fi(f);
        auto icon = p.icon(fi);
        auto item = new QListWidgetItem(listWidget);
        item->setText(f);
        item->setIcon(icon);
        listWidget->addItem(item);
    }
}

void FilesTreeDialog::on_treeView_clicked(const QModelIndex &index)
{
    QFileIconProvider p;
    listWidget->clear();

    for (const auto &f : mTreeModel->data(index)) {
        QFileInfo fi(f);
        auto icon = p.icon(fi);
        auto item = new QListWidgetItem(listWidget);
        item->setText(f);
        item->setIcon(icon);
        listWidget->addItem(item);
    }
}

void FilesTreeDialog::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    auto path = mTreeModel->fullPath(treeView->currentIndex());

    if (path == QLatin1Char('/'))
        path = listWidget->currentItem()->text();
    else
        path += QLatin1Char('/') + listWidget->currentItem()->text();

    mActions->setFilePath(path);
    mActions->popup(listWidget->mapToGlobal(pos));
}
