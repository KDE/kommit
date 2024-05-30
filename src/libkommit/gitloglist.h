/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libkommit_export.h"
#include <QList>
#include <QMap>

namespace Git
{

class Manager;
class Commit;
class LIBKOMMIT_EXPORT LogList : public QList<Commit *>
{
public:
    LogList();
    explicit LogList(QString branch);

    void load(Git::Manager *git);
    void initGraph();

    Q_REQUIRED_RESULT const QString &branch() const;
    void setBranch(const QString &newBranch);

    Commit *findByHash(const QString &hash, int *index = nullptr) const;

private:
    Q_REQUIRED_RESULT LIBKOMMIT_NO_EXPORT QString branchName(const QString &refLog);
    LIBKOMMIT_NO_EXPORT void initChilds();
    QString mBranch;
    QStringList mBranches;
    QMap<QString, Commit *> mDataByCommitHashLong;
};
} // namespace Git
