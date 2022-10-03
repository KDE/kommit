/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libgitklient_export.h"
#include <QList>
#include <QMap>

namespace Git
{

class Log;
class LIBGITKLIENT_EXPORT LogList : public QList<Log *>
{
public:
    LogList();
    explicit LogList(QString branch);

    void load();
    void initGraph();

    const QString &branch() const;
    void setBranch(const QString &newBranch);

    Log *findByHash(const QString &hash, int *index = nullptr) const;

private:
    QString branchName(const QString &refLog);
    void initChilds();
    QString _branch;
    QList<Log *> _data;
    QStringList _branches;
    QMap<QString, Log *> _dataByCommitHashLong;
    QMap<QString, Log *> _dataByCommitHashShort;
};
} // namespace Git
