/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remoteswidget.h"
#include "actions/remotesactions.h"
#include "gitmanager.h"
#include "models/remotesmodel.h"

RemotesWidget::RemotesWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);

    init(git);
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

    mActions->setRemoteName(remote->name);
    labelRemoteName->setText(remote->name);
    labelFetchUrl->setText(remote->fetchUrl);
    labelPushUrl->setText(remote->pushUrl);
    labelDefaultBranch->setText(remote->headBranch);
    treeWidget->clear();

    for (const auto &rb : std::as_const(remote->branches)) {
        auto item = new QTreeWidgetItem(treeWidget);

        item->setText(0, rb.name);
        item->setText(1, rb.remotePushBranch);
        item->setText(2, rb.remotePullBranch);
        item->setText(3, rb.statusText());

        treeWidget->addTopLevelItem(item);
    }
}

void RemotesWidget::slotListViewCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto remote = mModel->fromIndex(listView->currentIndex());
    if (!remote)
        return;

    mActions->setRemoteName(remote->name);
    mActions->popup();
}

void RemotesWidget::init(Git::Manager *git)
{
    mModel = git->remotesModel();
    listView->setModelColumn(0);
    listView->setModel(mModel);

    mActions = new RemotesActions(git, this);
    pushButtonAdd->setAction(mActions->actionCreate());
    pushButtonRemove->setAction(mActions->actionRemove());

    connect(listView, &ListView::itemActivated, this, &RemotesWidget::slotListViewItemActivated);
    connect(listView, &QListView::customContextMenuRequested, this, &RemotesWidget::slotListViewCustomContextMenuRequested);
}
