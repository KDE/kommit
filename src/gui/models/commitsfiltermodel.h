/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSortFilterProxyModel>

class CommitsModel;

class CommitsFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CommitsFilterModel(CommitsModel *sourceModel, QObject *parent = nullptr);

    [[nodiscard]] const QString &filterTerm() const;
    void setFilterTerm(const QString &newFilterTerm);

protected:
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString mFilterTerm;
    CommitsModel *const mSourceModel;
};
