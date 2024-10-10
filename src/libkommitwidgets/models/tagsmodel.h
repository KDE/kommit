/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractgititemsmodel.h"
#include "libkommitwidgets_export.h"

namespace Git
{

class Tag;
}

class LIBKOMMITWIDGETS_EXPORT TagsModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    enum class TagsModelRoles {
        Name,
        Subject,
        Tagger,
        Time,
        LastColumn = Time,
    };
    Q_ENUM(TagsModelRoles)
    explicit TagsModel(Git::Repository *git, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QSharedPointer<Git::Tag> fromIndex(const QModelIndex &index) const;

    void clear() override;

protected:
    void reload() override;

private:
    QList<QSharedPointer<Git::Tag>> mData;
};
