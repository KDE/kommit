/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "treewidget.h"

TreeWidget::TreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
}

void TreeWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    emit itemActivated(current);
}
