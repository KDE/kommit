/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filestatus.h"

#include "filedelta.h"

#include <utility>

namespace Git
{

class FileStatusPrivate
{
public:
    FileStatusPrivate();
    FileStatusPrivate(const QString &oldName, const QString &newName, ChangeStatus status);
    FileStatusPrivate(const StatusFlags &status, const DiffDelta &headToIndex, const DiffDelta &indexToWorkdir);

    QString fullPath;
    QString oldName;
    QString newName;
    ChangeStatus status;

    FileStatus::Status mStatus;

    StatusFlags statusFlags;
    DiffDelta headToIndex;
    DiffDelta indexToWorkdir;
};

FileStatus::FileStatus() = default;

FileStatus::FileStatus(StatusFlags status, DiffDelta headToIndex, DiffDelta indexToWorkdir)
    : d{new FileStatusPrivate{status, headToIndex, indexToWorkdir}}
{
}

FileStatus::FileStatus(QString name, FileStatus::Status status)
    : d{new FileStatusPrivate}
{
    d->newName = name;
    d->mStatus = status;
}

FileStatus::FileStatus(const QString &oldName, const QString &newName, ChangeStatus status)
    : d{new FileStatusPrivate{oldName, newName, status}}
{
}

const QString &FileStatus::name() const
{
    switch (d->statusFlags) {
    }
    if (!d->headToIndex.isNull())
        return d->headToIndex.newFile().path();

    if (!d->indexToWorkdir.isNull())
        return d->indexToWorkdir.newFile().path();
    return {};
}

FileStatus::Status FileStatus::status() const
{
    return d->mStatus;
}

void FileStatus::setStatus(const QString &x, const QString &y)
{
    if (x == QLatin1Char('M') || y == QLatin1Char('M'))
        d->mStatus = Modified;
    else if (x == QLatin1Char('A'))
        d->mStatus = Added;
    else if (x == QLatin1Char('D'))
        d->mStatus = Removed;
    else if (x == QLatin1Char('R'))
        d->mStatus = Renamed;
    else if (x == QLatin1Char('C'))
        d->mStatus = Copied;
    else if (x == QLatin1Char('U'))
        d->mStatus = UpdatedButInmerged;
    else if (x == QLatin1Char('?'))
        d->mStatus = Untracked;
    else if (x == QLatin1Char('!'))
        d->mStatus = Ignored;
    else
        d->mStatus = Unknown;
}

void FileStatus::setName(const QString &newName)
{
    // mName = newName;
}

bool FileStatus::operator==(const FileStatus &other)
{
    return false; // mName == other.name();
}

bool operator==(const FileStatus &f1, const FileStatus &f2)
{
    return f1.name() == f2.name();
}

FileStatusPrivate::FileStatusPrivate()
    : headToIndex{nullptr}
    , indexToWorkdir{nullptr}
{
}

FileStatusPrivate::FileStatusPrivate(const QString &oldName, const QString &newName, ChangeStatus status)
    : oldName(oldName)
    , newName(newName)
    , status(status)
    , headToIndex{nullptr}
    , indexToWorkdir{nullptr}
{
}

FileStatusPrivate::FileStatusPrivate(const StatusFlags &status, const DiffDelta &headToIndex, const DiffDelta &indexToWorkdir)
    : statusFlags(status)
    , headToIndex(headToIndex)
    , indexToWorkdir(indexToWorkdir)
{
}
}
