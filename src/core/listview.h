/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

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

