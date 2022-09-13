/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduleswidget.h"
#include "actions/submoduleactions.h"
#include "dialogs/submoduleinfodialog.h"
#include "git/gitmanager.h"
#include "git/models/submodulesmodel.h"

SubmodulesWidget::SubmodulesWidget(QWidget *parent)
    : WidgetBase(parent)
{
    setupUi(this);
    mActions = new SubmoduleActions(Git::Manager::instance(), this);

    pushButtonAddNew->setAction(mActions->actionCreate());
    pushButtonUpdate->setAction(mActions->actionUpdate());

    _model = Git::Manager::instance()->submodulesModel();
    treeView->setModel(_model);
}

SubmodulesWidget::SubmodulesWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);
    mActions = new SubmoduleActions(git, this);

    pushButtonAddNew->setAction(mActions->actionCreate());
    pushButtonUpdate->setAction(mActions->actionUpdate());

    _model = git->submodulesModel();
    treeView->setModel(_model);
}

void SubmodulesWidget::saveState(QSettings &settings) const
{
    save(settings, treeView);
}

void SubmodulesWidget::restoreState(QSettings &settings)
{
    restore(settings, treeView);
}

void SubmodulesWidget::reload()
{
}

void SubmodulesWidget::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)

    if (!treeView->currentIndex().isValid())
        return;

    auto s = _model->fromIndex(treeView->currentIndex());
    if (!s)
        return;
    mActions->setSubModuleName(s->path());
    mActions->popup();
}

void SubmodulesWidget::on_treeView_activated(const QModelIndex &index)
{
    auto s = _model->fromIndex(index);
    if (!s)
        return;
    mActions->setSubModuleName(s->path());
}
