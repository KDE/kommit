/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

class ListView : public QListView
{
    Q_OBJECT

public:
    explicit ListView(QWidget *parent = nullptr);

protected slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

signals:
    void itemActivated(const QModelIndex &index);
};

#endif // LISTVIEW_H
