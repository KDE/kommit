/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stasheswidget.h"
#include "actions/stashactions.h"
#include "gitmanager.h"
#include "models/stashesmodel.h"

StashesWidget::StashesWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mActions(new StashActions(git, this))
    , mModel(git->stashesModel())
{
    setupUi(this);
    init();
}

void StashesWidget::init()
{
    pushButtonApply->setAction(mActions->actionApply());
    pushButtonRemoveSelected->setAction(mActions->actionDrop());
    pushButtonCreateNew->setAction(mActions->actionNew());

    treeView->setModel(mModel);

    connect(treeView, &QTreeView::customContextMenuRequested, this, &StashesWidget::slotTreeViewCustomContextMenuRequested);
    connect(treeView, &TreeView::itemActivated, this, &StashesWidget::slotTreeViewItemActivated);
}

void StashesWidget::slotTreeViewCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)

    auto stash = mModel->fromIndex(treeView->currentIndex());

    if (!stash)
        return;

    mActions->setStash(stash);
    mActions->popup();
}

void StashesWidget::slotTreeViewItemActivated(const QModelIndex &index)
{
    auto stash = mModel->fromIndex(index);

    if (!stash)
        return;

    mActions->setStash(stash);
}

void StashesWidget::saveState(QSettings &settings) const
{
    save(settings, treeView);
}

void StashesWidget::restoreState(QSettings &settings)
{
    restore(settings, treeView);
}

#include "moc_stasheswidget.cpp"
