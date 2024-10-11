/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduleswidget.h"
#include "actions/submoduleactions.h"

#include <core/repositorydata.h>
#include <models/submodulesmodel.h>

#include <Kommit/SubModule>
#include <Kommit/Repository>

SubmodulesWidget::SubmodulesWidget(RepositoryData *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mActions(new SubmoduleActions(git->manager(), this))
    , mModel(git->submodulesModel())
{
    setupUi(this);

    pushButtonAddNew->setAction(mActions->actionCreate());
    pushButtonUpdate->setAction(mActions->actionUpdate());

    treeView->setModel(mModel);

    connect(treeView, &QTreeView::customContextMenuRequested, this, &SubmodulesWidget::slotTreeViewCustomContextMenuRequested);
    connect(treeView, &TreeView::itemActivated, this, &SubmodulesWidget::slotTreeViewActivated);
}

void SubmodulesWidget::saveState(QSettings &settings) const
{
    save(settings, treeView);
}

void SubmodulesWidget::restoreState(QSettings &settings)
{
    restore(settings, treeView);
}

void SubmodulesWidget::slotTreeViewCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)

    if (!treeView->currentIndex().isValid())
        return;

    auto s = mModel->fromIndex(treeView->currentIndex());
    if (!s)
        return;
    mActions->setSubmodule(s);
    mActions->popup();
}

void SubmodulesWidget::slotTreeViewActivated(const QModelIndex &index)
{
    auto s = mModel->fromIndex(index);
    if (!s)
        return;
    mActions->setSubmodule(s);
}

#include "moc_submoduleswidget.cpp"
