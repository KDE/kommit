/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduleswidget.h"
#include "actions/submoduleactions.h"

#include <entities/submodule.h>
#include <gitmanager.h>
#include <models/submodulesmodel.h>

SubmodulesWidget::SubmodulesWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mActions(new SubmoduleActions(git, this))
{
    setupUi(this);

    pushButtonAddNew->setAction(mActions->actionCreate());
    pushButtonUpdate->setAction(mActions->actionUpdate());

    mModel = git->submodulesModel();
    treeView->setModel(mModel);

    connect(treeView, &QTreeView::customContextMenuRequested, this, &SubmodulesWidget::slotTreeViewCustomContextMenuRequested);
    connect(treeView, &QTreeView::activated, this, &SubmodulesWidget::slotTreeViewActivated);
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
    mActions->setSubModuleName(s->path());
    mActions->popup();
}

void SubmodulesWidget::slotTreeViewActivated(const QModelIndex &index)
{
    auto s = mModel->fromIndex(index);
    if (!s)
        return;
    mActions->setSubModuleName(s->path());
}

#include "moc_submoduleswidget.cpp"
