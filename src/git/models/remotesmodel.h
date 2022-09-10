/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

//
// Created by hamed on 25.03.22.
//

#pragma once

#include "abstractgititemsmodel.h"

namespace Git {
class Remote;

class RemotesModel : public AbstractGitItemsModel
{
    Q_OBJECT
    QList<Remote *> _data;

public:
    explicit RemotesModel(Manager *git, QObject *parent = nullptr);
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Remote *fromIndex(const QModelIndex &index);
    Remote *findByName(const QString &name);

    void rename(const QString &oldName, const QString &newName);
    void setUrl(const QString &remoteName, const QString &newUrl);

protected:
    void fill() override;
};
} // namespace Git

