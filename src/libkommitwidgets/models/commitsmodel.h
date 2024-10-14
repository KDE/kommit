/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractgititemsmodel.h"
#include "gitgraphlane.h"
#include "libkommitwidgets_export.h"

#include <QCalendar>
#include <QScopedPointer>
#include <QSet>

namespace Git
{
class Branch;
class Commit;
class Repository;
}

class CommitsModelPrivate;
class LIBKOMMITWIDGETS_EXPORT CommitsModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    enum class LogMatchType { ExactMatch, BeginMatch };
    explicit CommitsModel(Git::Repository *git, QObject *parent = nullptr);
    ~CommitsModel();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QSharedPointer<Git::Commit> at(int index) const;
    QSharedPointer<Git::Commit> fromIndex(const QModelIndex &index) const;
    QVector<GraphLane> lanesFromIndex(const QModelIndex &index) const;

    QModelIndex findIndexByHash(const QString &hash) const;
    QSharedPointer<Git::Commit> findLogByHash(const QString &hash, LogMatchType matchType = LogMatchType::ExactMatch) const;

    [[nodiscard]] QSharedPointer<Git::Branch> branch() const;
    void setBranch(QSharedPointer<Git::Branch> newBranch);

    [[nodiscard]] bool fullDetails() const;
    void setFullDetails(bool newFullDetails);

    [[nodiscard]] QString calendarType() const;
    void setCalendarType(const QString &newCalendarType);

    void clear() override;

protected:
    void reload() override;

private:
    QScopedPointer<CommitsModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(CommitsModel)
};
