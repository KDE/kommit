/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remotedetailswidget.h"
#include "git/gitmanager.h"
#include "git/gitremote.h"
#include "git/models/remotesmodel.h"

Git::Manager *RemoteDetailsWidget::git() const
{
    return mGit;
}

void RemoteDetailsWidget::setGit(Git::Manager *newGit)
{
    mGit = newGit;
    //    listWidget->clear();
    listView->setModel(mGit->remotesModel());
}

void RemoteDetailsWidget::reload()
{
    //    listWidget->clear();
    //    listWidget->addItems(_git->remotes());
}

// void RemoteDetailsWidget::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
//{
//     Q_UNUSED(previous)
//     setBranch(current->text());
// }

RemoteDetailsWidget::RemoteDetailsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    mGit = Git::Manager::instance();
    connect(mGit, &Git::Manager::pathChanged, this, &RemoteDetailsWidget::reload);
}

const QString &RemoteDetailsWidget::branch() const
{
    return mBranch;
}

void RemoteDetailsWidget::setBranch(const QString &newBranch)
{
    mBranch = newBranch;

    if (!mGit)
        mGit = Git::Manager::instance();
    auto info = mGit->remoteDetails(newBranch);

    labelRemoteName->setText(info.name);
    labelFetchUrl->setText(info.fetchUrl);
    labelPushUrl->setText(info.pushUrl);
    labelDefaultBranch->setText(info.headBranch);

    for (const auto &rb : qAsConst(info.branches)) {
        auto item = new QTreeWidgetItem(treeWidget);

        item->setText(0, rb.name);
        item->setText(1, rb.remotePushBranch);
        item->setText(2, rb.remotePullBranch);
        item->setText(3, rb.statusText());

        treeWidget->addTopLevelItem(item);
    }
}
