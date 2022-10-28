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
    tableViewAuthors->setSortingEnabled(true);
    tableViewAuthors->setModel(mGit->authorsModelFilterProxyModel());
    tableViewAuthors->sortByColumn(Git::AuthorsModel::Commits, Qt::DescendingOrder);
}

void AuthorsWidget::saveState(QSettings &settings) const
{
    save(settings, tableViewAuthors);
}

void AuthorsWidget::restoreState(QSettings &settings)
{
    restore(settings, tableViewAuthors);
}
