/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

//
// Created by hamed on 25.03.22.
//

#pragma once
#include "abstractgititemsmodel.h"
#include "libkommit_export.h"

#include <QCalendar>

namespace Git
{

class Log;
class Manager;
class AuthorsModel;
class LIBKOMMIT_EXPORT LogsModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    enum class LogMatchType { ExactMatch, BeginMatch };
    explicit LogsModel(Manager *git, AuthorsModel *authorsModel = nullptr, QObject *parent = nullptr);
    ~LogsModel() override;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Log *at(int index) const;
    Log *fromIndex(const QModelIndex &index) const;
    QModelIndex findIndexByHash(const QString &hash) const;
    Git::Log *findLogByHash(const QString &hash, LogMatchType matchType = LogMatchType::ExactMatch) const;

    Q_REQUIRED_RESULT const QString &branch() const;
    void setBranch(const QString &newBranch);

    bool fullDetails() const;
    void setFullDetails(bool newFullDetails);

    Q_REQUIRED_RESULT QString calendarType() const;
    void setCalendarType(const QString &newCalendarType);

protected:
    void fill() override;

private:
    bool mFullDetails{false};
    void initChilds();
    void initGraph();

    QString mBranch;
    QList<Log *> mData;
    QStringList mBranches;
    QMap<QString, Log *> mDataByCommitHashLong;
    QMap<QString, Log *> mDataByCommitHashShort;
    AuthorsModel *mAuthorsModel;
    QCalendar mCalendar;
};
}
