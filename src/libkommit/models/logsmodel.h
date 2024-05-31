/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractgititemsmodel.h"
#include "libkommit_export.h"

#include <QCalendar>
#include <QSet>

namespace Git
{

class Commit;
class Manager;

class LIBKOMMIT_EXPORT LogsModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    enum class LogMatchType { ExactMatch, BeginMatch };
    explicit LogsModel(Manager *git, QObject *parent = nullptr);
    ~LogsModel() override;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QSharedPointer<Commit> at(int index) const;
    QSharedPointer<Commit> fromIndex(const QModelIndex &index) const;
    QModelIndex findIndexByHash(const QString &hash) const;
    QSharedPointer<Commit> findLogByHash(const QString &hash, LogMatchType matchType = LogMatchType::ExactMatch) const;

    Q_REQUIRED_RESULT const QString &branch() const;
    void setBranch(const QString &newBranch);

    Q_REQUIRED_RESULT bool fullDetails() const;
    void setFullDetails(bool newFullDetails);

    Q_REQUIRED_RESULT QString calendarType() const;
    void setCalendarType(const QString &newCalendarType);

protected:
    void fill() override;

private:
    LIBKOMMIT_NO_EXPORT void initChilds();
    LIBKOMMIT_NO_EXPORT void initGraph();

    bool mFullDetails{false};
    QString mBranch;
    QList<QSharedPointer<Commit>> mData;
    QStringList mBranches;
    QMap<QString, QSharedPointer<Commit>> mDataByCommitHashLong;
    QMap<QString, QSharedPointer<Commit>> mDataByCommitHashShort;
    QCalendar mCalendar;
    QSet<QString> mSeenHases;
};
}
