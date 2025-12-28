/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <Kommit/DiffDelta>
#include <QObject>
#include <QString>

#include "types.h"

namespace Git
{

class FileStatusPrivate;
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
    FileStatus(StatusFlags status, DiffDelta headToIndex, DiffDelta indexToWorkdir);
    explicit FileStatus(QString name, Status status);
    FileStatus(const QString &oldName, const QString &newName, ChangeStatus status);

    [[nodiscard]] const QString &name() const;
    [[nodiscard]] Status status() const;

    // void parseStatusLine(const QString &line)
    // {
    //     const auto statusX = line.at(0);
    //     const auto statusY = line.at(1);
    //     const auto fileName = line.mid(3);
    //     d->newName = fileName;

    //     setStatus(statusX, statusY);
    // }

    // void setStatus(Status status)
    // {
    //     mStatus = status;
    // }
    void setStatus(const QString &x, const QString &y = QString());
    void setName(const QString &newName);

    bool operator==(const FileStatus &other);

private:
    friend class Repository;

    QSharedPointer<FileStatusPrivate> d;
};
LIBKOMMIT_EXPORT bool operator==(const FileStatus &f1, const FileStatus &f2);
}

Q_DECLARE_METATYPE(Git::FileStatus)
Q_DECLARE_TYPEINFO(Git::FileStatus, Q_MOVABLE_TYPE);
