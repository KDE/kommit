/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef COMMITSFILTERMODEL_H
#define COMMITSFILTERMODEL_H

#include <QSortFilterProxyModel>

namespace Git
{
class LogsModel;
};

class CommitsFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CommitsFilterModel(Git::LogsModel *sourceModel, QObject *parent = nullptr);

    const QString &filterTerm() const;
    void setFilterTerm(const QString &newFilterTerm);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString mFilterTerm;
    Git::LogsModel *mSourceModel;
};

#endif // COMMITSFILTERMODEL_H
