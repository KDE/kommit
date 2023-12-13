/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagswidget.h"

#include "actions/tagsactions.h"
#include "models/tagsmodel.h"

#include <entities/tag.h>
#include <gitmanager.h>

#include <QSortFilterProxyModel>

TagsWidget::TagsWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mActions(new TagsActions(git, this))
    , mModel(git->tagsModel())
{
    setupUi(this);
    treeViewTags->setSortingEnabled(true);
    auto sortFilterProxyModel = new QSortFilterProxyModel(this);
    sortFilterProxyModel->setSourceModel(mModel);
    treeViewTags->setModel(sortFilterProxyModel);

    pushButtonAddTag->setAction(mActions->actionCreate());
    pushButtonRemove->setAction(mActions->actionRemove());
    pushButtonCheckout->setAction(mActions->actionCheckout());
    pushButtonPush->setAction(mActions->actionPush());

    connect(treeViewTags, &QTreeView::customContextMenuRequested, this, &TagsWidget::slotTreeViewTagsCustomContextMenuRequested);
    connect(treeViewTags, &TreeView::itemActivated, this, &TagsWidget::slotTreeViewTagsItemActivated);
    treeViewTags->sortByColumn(Git::TagsModel::Name, Qt::DescendingOrder);
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

#include "moc_tagswidget.cpp"
