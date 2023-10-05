/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractgititemsmodel.h"
#include "libkommit_export.h"

#include <QCalendar>

namespace Git
{

class Commit;
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

    Commit *at(int index) const;
    Commit *fromIndex(const QModelIndex &index) const;
    QModelIndex findIndexByHash(const QString &hash) const;
    Git::Commit *findLogByHash(const QString &hash, LogMatchType matchType = LogMatchType::ExactMatch) const;

    Q_REQUIRED_RESULT const QString &branch() const;
    void setBranch(const QString &newBranch);

    Q_REQUIRED_RESULT bool fullDetails() const;
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
    QList<Commit *> mData;
    QStringList mBranches;
    QMap<QString, Commit *> mDataByCommitHashLong;
    QMap<QString, Commit *> mDataByCommitHashShort;
    AuthorsModel *mAuthorsModel;
    QCalendar mCalendar;
};
}
