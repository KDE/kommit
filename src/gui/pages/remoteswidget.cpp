/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remoteswidget.h"
#include "actions/remotesactions.h"
#include "models/remotesmodel.h"

#include <entities/branch.h>
#include <gitmanager.h>

RemotesWidget::RemotesWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mModel(git->remotesModel())
    , mActions(new RemotesActions(git, this))

{
    setupUi(this);
    init();
}

void RemotesWidget::saveState(QSettings &settings) const
{
    save(settings, splitter);
    save(settings, treeWidget);
}

void RemotesWidget::restoreState(QSettings &settings)
{
    restore(settings, splitter);
    restore(settings, treeWidget);
}

void RemotesWidget::slotListViewItemActivated(const QModelIndex &index)
{
    auto remote = mModel->fromIndex(index);
    if (!remote)
        return;

    mActions->setRemoteName(remote->name());
    labelRemoteName->setText(remote->name());
    labelFetchUrl->setText(remote->fetchUrl());
    labelPushUrl->setText(remote->pushUrl());
    labelDefaultBranch->setText(remote->defaultBranch());
    treeWidget->clear();
    //    for (auto &ref : remote->refSpecList()) {
    //        auto item = new QTreeWidgetItem(treeWidget);

    //        item->setText(0, ref->name());
    //        item->setText(1, ref->source());
    //        item->setText(2, ref->destionation());
    //        item->setText(3, ref->direction() == Git::RefSpec::Direction::DirectionFetch ? "Fetch" : "Push");

    //        treeWidget->addTopLevelItem(item);
    //    }
    for (const auto &rb : remote->branches()) {
        auto item = new QTreeWidgetItem(treeWidget);

        item->setText(0, rb->name());
        item->setText(1, rb->refName());
        item->setText(2, rb->upStreamName());
        item->setText(3, rb->isHead() ? i18n("Update") : QString());

        treeWidget->addTopLevelItem(item);
    }
}

void RemotesWidget::slotListViewCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto remote = mModel->fromIndex(listView->currentIndex());
    if (!remote)
        return;

    mActions->setRemoteName(remote->name());
    mActions->popup();
}

void RemotesWidget::init()
{
    listView->setModelColumn(0);
    listView->setModel(mModel);

    pushButtonAdd->setAction(mActions->actionCreate());
    pushButtonRemove->setAction(mActions->actionRemove());

    connect(listView, &ListView::itemActivated, this, &RemotesWidget::slotListViewItemActivated);
    connect(listView, &QListView::customContextMenuRequested, this, &RemotesWidget::slotListViewCustomContextMenuRequested);
}

#include "moc_remoteswidget.cpp"
