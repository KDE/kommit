/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libkommit_export.h"
#include <QList>
#include <QMap>
#include <entities/commit.h>

namespace Git
{

class Repository;
class Commit;
// TODO: remove this class
class LIBKOMMIT_EXPORT LogList : public QList<Commit *>
{
public:
    LogList();
    explicit LogList(QString branch);

    void load(Git::Repository *git);
    void initGraph();

    [[nodiscard]] const QString &branch() const;
    void setBranch(const QString &newBranch);

    Commit *findByHash(const QString &hash, int *index = nullptr) const;

private:
    [[nodiscard]] LIBKOMMIT_NO_EXPORT QString branchName(const QString &refLog);
    QString mBranch;
    QStringList mBranches;
    QMap<QString, Commit *> mDataByCommitHashLong;
};
} // namespace Git
