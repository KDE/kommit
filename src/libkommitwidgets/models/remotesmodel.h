/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractgititemsmodel.h"
#include "libkommitwidgets_export.h"

#include <Kommit/Remote>

class LIBKOMMITWIDGETS_EXPORT RemotesModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    explicit RemotesModel(Git::Repository *git);
    ~RemotesModel() override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Git::Remote fromIndex(const QModelIndex &index);
    Git::Remote findByName(const QString &name);

    void rename(const QString &oldName, const QString &newName);
    void setUrl(const QString &remoteName, const QString &newUrl);

    void clear() override;

protected:
    void reload() override;

private:
    QList<Git::Remote> mData;
};
