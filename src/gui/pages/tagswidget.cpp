/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagswidget.h"

#include "actions/tagsactions.h"
#include "models/tagsmodel.h"

#include <core/repositorydata.h>
#include <entities/tag.h>
#include <repository.h>

#include <QSortFilterProxyModel>

TagsWidget::TagsWidget(RepositoryData *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mActions(new TagsActions(git->manager(), this))
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
    treeViewTags->sortByColumn(static_cast<int>(TagsModel::TagsModelRoles::Name), Qt::DescendingOrder);
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
    auto tag = mModel->fromIndex(treeViewTags->currentIndex());
    if (tag) {
        mActions->setTag(tag);
        mActions->popup();
    }
}

void TagsWidget::slotTreeViewTagsItemActivated(const QModelIndex &index)
{
    auto tag = mModel->fromIndex(index);
    if (tag)
        mActions->setTag(tag);
}

#include "moc_tagswidget.cpp"
