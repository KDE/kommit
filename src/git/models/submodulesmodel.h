/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libgitklient_export.h"
#include "abstractgititemsmodel.h"

namespace Git
{

class Submodule;
class LIBGITKLIENT_EXPORT SubmodulesModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    explicit SubmodulesModel(Git::Manager *git, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool append(Submodule *module);
    Submodule *fromIndex(const QModelIndex &index);

protected:
    void fill() override;
private:
    QList<Submodule *> mData;
};

} // namespace Git
