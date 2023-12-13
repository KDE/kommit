/*
SPDX-FileCopyrightText: 2022 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "authorswidget.h"

#include "gitmanager.h"
#include "models/authorsmodel.h"

#include <QSortFilterProxyModel>

AuthorsWidget::AuthorsWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);
    treeViewAuthors->setSortingEnabled(true);

    auto sortingModel = new QSortFilterProxyModel(this);
    sortingModel->setSourceModel(mGit->authorsModel());
    treeViewAuthors->setModel(sortingModel);
}

void AuthorsWidget::saveState(QSettings &settings) const
{
    save(settings, treeViewAuthors);
}

void AuthorsWidget::restoreState(QSettings &settings)
{
    restore(settings, treeViewAuthors);
}

#include "moc_authorswidget.cpp"
