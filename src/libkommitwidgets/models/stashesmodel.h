/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractgititemsmodel.h"
#include "libkommitwidgets_export.h"

namespace Git
{

class Repository;
class Stash;
}

class LIBKOMMITWIDGETS_EXPORT StashesModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    enum StashesModelRoles {
        Subject,
        CommitHash,
        AuthorName,
        AuthorEmail,
        Time,
        LastColumn = Time,
    };
    explicit StashesModel(Git::Repository *git, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Git::Stash fromIndex(const QModelIndex &index) const;

    void clear() override;

protected:
    void reload() override;

private:
    QList<Git::Stash> mData;
};
