/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagswidget.h"

#include "git/gitmanager.h"
#include "git/gittag.h"
#include "git/models/tagsmodel.h"

#include "actions/tagsactions.h"
#include "dialogs/taginfodialog.h"

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
}

void TagsWidget::saveState(QSettings &settings) const
{
    save(settings, treeViewTags);
}

void TagsWidget::restoreState(QSettings &settings)
{
    restore(settings, treeViewTags);
}

void TagsWidget::on_treeViewTags_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto item = mModel->fromIndex(treeViewTags->currentIndex());
    if (item) {
        mActions->setTagName(item->name());
        mActions->popup();
    }
}

void TagsWidget::on_treeViewTags_itemActivated(const QModelIndex &index)
{
    auto item = mModel->fromIndex(index);
    if (item)
        mActions->setTagName(item->name());
}
