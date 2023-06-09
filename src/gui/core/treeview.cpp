/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "treeview.h"

TreeView::TreeView(QWidget *parent)
    : QTreeView(parent)
{
}

void TreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    Q_EMIT itemActivated(current);
}

#include "moc_treeview.cpp"
