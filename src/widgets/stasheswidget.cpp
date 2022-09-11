/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stasheswidget.h"
#include "actions/stashactions.h"
#include "git/gitmanager.h"
#include "git/models/stashesmodel.h"

StashesWidget::StashesWidget(QWidget *parent)
    : WidgetBase(parent)
{
    setupUi(this);
    init(Git::Manager::instance());
}

StashesWidget::StashesWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);
    init(git);
}

void StashesWidget::init(Git::Manager *git)
{
    _actions = new StashActions(git, this);

    pushButtonApply->setAction(_actions->actionApply());
    pushButtonRemoveSelected->setAction(_actions->actionDrop());
    pushButtonCreateNew->setAction(_actions->actionNew());

    _model = git->stashesModel();
    treeView->setModel(_model);
}

void StashesWidget::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)

    auto stash = _model->fromIndex(treeView->currentIndex());

    if (!stash)
        return;

    _actions->setStashName(stash->name());
    _actions->popup();
}

void StashesWidget::on_treeView_itemActivated(const QModelIndex &index)
{
    auto stash = _model->fromIndex(index);

    if (!stash)
        return;

    _actions->setStashName(stash->name());
}

void StashesWidget::saveState(QSettings &settings) const
{
    save(settings, treeView);
}

void StashesWidget::restoreState(QSettings &settings)
{
    restore(settings, treeView);
}
