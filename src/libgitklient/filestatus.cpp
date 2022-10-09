/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filestatus.h"
#include <utility>

FileStatus::FileStatus() = default;

FileStatus::FileStatus(QString name, FileStatus::Status status)
    : mName(std::move(name))
    , mStatus(status)
{
}

const QString &FileStatus::name() const
{
    return mName;
}

FileStatus::Status FileStatus::status() const
{
    return mStatus;
}

void FileStatus::parseStatusLine(const QString &line)
{
    auto statusX = line.at(0);
    auto statusY = line.at(1);
    auto fileName = line.mid(3);
    mName = fileName;

    setStatus(statusX, statusY);

    //    qCDebug(GITKLIENTLIB_LOG) << "***=" << line << _status << statusX << statusY;
}

const QString &FileStatus::fullPath() const
{
    return mFullPath;
}

void FileStatus::setFullPath(const QString &newFullPath)
{
    mFullPath = newFullPath;
}

void FileStatus::setStatus(const QString &x, const QString &y)
{
    if (x == QLatin1Char('M') || y == QLatin1Char('M'))
        mStatus = Modified;
    else if (x == QLatin1Char('A'))
        mStatus = Added;
    else if (x == QLatin1Char('D'))
        mStatus = Removed;
    else if (x == QLatin1Char('R'))
        mStatus = Renamed;
    else if (x == QLatin1Char('C'))
        mStatus = Copied;
    else if (x == QLatin1Char('U'))
        mStatus = UpdatedButInmerged;
    else if (x == QLatin1Char('?'))
        mStatus = Untracked;
    else if (x == QLatin1Char('!'))
        mStatus = Ignored;
    else
        mStatus = Unknown;
}

void FileStatus::setName(const QString &newName)
{
    mName = newName;
}

bool operator==(const FileStatus &f1, const FileStatus &f2)
{
    return f1.name() == f2.name();
}
