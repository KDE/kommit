/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include <QObject>
#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT FileStatus
{
public:
    enum Status {
        Unknown = 0,
        Unmodified = 1,
        Modified = 2,
        Added = 3,
        Removed = 4,
        Renamed = 5,
        Copied = 6,
        UpdatedButInmerged = 7,
        Ignored = 8,
        Untracked = 9,
        NoGit = 10
    };

    FileStatus();
    explicit FileStatus(QString name, Status status);

    const QString &name() const;
    [[nodiscard]] Status status() const;

    void parseStatusLine(const QString &line);
    [[nodiscard]] const QString &fullPath() const;

    void setFullPath(const QString &newFullPath);

    void setStatus(Status status);
    void setStatus(const QString &x, const QString &y = QString());
    void setName(const QString &newName);

    bool operator==(const FileStatus &other);

private:
    QString mFullPath;
    QString mName;
    Status mStatus;

    friend class Manager;
};
LIBKOMMIT_EXPORT bool operator==(const FileStatus &f1, const FileStatus &f2);
}

Q_DECLARE_METATYPE(Git::FileStatus)
Q_DECLARE_TYPEINFO(Git::FileStatus, Q_MOVABLE_TYPE);
