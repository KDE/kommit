/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractgititemsmodel.h"

#include <QMutex>

namespace Git
{

struct Author {
    QString email;
    QString name;

    int commits{0};
    int authoredCommits{0};
    int tags{0};
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

private:
    QList<Author *> mData;
    QMutex mDataMutex;

protected:
    void fill() override;
};

} // namespace Git
