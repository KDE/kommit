/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractgititemsmodel.h"

namespace Git
{

class Manager;
class Stash;
class StashesModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    explicit StashesModel(Manager *git, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Stash *fromIndex(const QModelIndex &index) const;

protected:
    void fill() override;
private:
    QList<Stash *> mData;
};

}
