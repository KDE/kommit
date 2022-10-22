/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagswidget.h"

#include "gitmanager.h"
#include "gittag.h"
#include "models/tagsmodel.h"

#include "actions/tagsactions.h"

// TODO: remove ctor without git input
TagsWidget::TagsWidget(QWidget *parent)
    : WidgetBase(parent)
{
    setupUi(this);
    mModel = Git::Manager::instance()->tagsModel();
    treeViewTags->setModel(mModel);
    mActions = new TagsActions(Git::Manager::instance(), this);
    pushButtonAddTag->setAction(mActions->actionCreate());
    pushButtonRemove->setAction(mActions->actionRemove());
    pushButtonCheckout->setAction(mActions->actionCheckout());
    pushButtonPush->setAction(mActions->actionPush());

    connect(treeViewTags, &QTreeView::customContextMenuRequested, this, &TagsWidget::slotTreeViewTagsCustomContextMenuRequested);
    connect(treeViewTags, &TreeView::itemActivated, this, &TagsWidget::slotTreeViewTagsItemActivated);
}

TagsWidget::TagsWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);
    mModel = git->tagsModel();
    treeViewTags->setModel(mModel);
    mActions = new TagsActions(git, this);
    pushButtonAddTag->setAction(mActions->actionCreate());
    pushButtonRemove->setAction(mActions->actionRemove());
    pushButtonCheckout->setAction(mActions->actionCheckout());
    pushButtonPush->setAction(mActions->actionPush());

    connect(treeViewTags, &QTreeView::customContextMenuRequested, this, &TagsWidget::slotTreeViewTagsCustomContextMenuRequested);
    connect(treeViewTags, &TreeView::itemActivated, this, &TagsWidget::slotTreeViewTagsItemActivated);
}

void TagsWidget::saveState(QSettings &settings) const
{
    save(settings, treeViewTags);
}

void TagsWidget::restoreState(QSettings &settings)
{
    restore(settings, treeViewTags);
}

void TagsWidget::slotTreeViewTagsCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto item = mModel->fromIndex(treeViewTags->currentIndex());
    if (item) {
        mActions->setTagName(item->name());
        mActions->popup();
    }
}

void TagsWidget::slotTreeViewTagsItemActivated(const QModelIndex &index)
{
    auto item = mModel->fromIndex(index);
    if (item)
        mActions->setTagName(item->name());
}
