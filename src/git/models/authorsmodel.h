/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractgititemsmodel.h"

namespace Git
{

struct Author {
    QString email;
    QString name;
};

class AuthorsModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    explicit AuthorsModel(Manager *git, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Author *findOrCreate(const QString &name, const QString &email);

protected:
    void fill() override;
private:
    QList<Author *> mData;
};

} // namespace Git
