/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractgititemsmodel.h"
#include "libkommitwidgets_export.h"

#include <QScopedPointer>

namespace Git
{
class Repository;
class Submodule;
}

class SubmodulesModelPrivate;
class LIBKOMMITWIDGETS_EXPORT SubmodulesModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    explicit SubmodulesModel(Git::Repository *git);
    ~SubmodulesModel();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool append(QSharedPointer<Git::Submodule> module);
    QSharedPointer<Git::Submodule> fromIndex(const QModelIndex &index);

    void reload() override;

private:
    QScopedPointer<SubmodulesModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(SubmodulesModel)
};
