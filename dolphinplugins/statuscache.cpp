/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "statuscache.h"

#include "manager.h"
#include <QFileInfo>

StatusCache::StatusCache() = default;
bool StatusCache::addPath(const QString &path)
{
    Git::MiniManager git(path);
    if (!git.isValid())
        return false;

    auto statuses = git.repoFilesStatus();

    for (const auto &s : std::as_const(statuses)) {
        mStatuses.insert(git.path() + QLatin1Char('/') + s.name(), s.status());
    }
    return true;
}

bool StatusCache::isInDir(const QString &dirPath, const QString &filePath)
{
    if (dirPath.endsWith(QLatin1Char('/')))
        return filePath.lastIndexOf(QLatin1Char('/')) == dirPath.size() - 1;
    return filePath.lastIndexOf(QLatin1Char('/')) == dirPath.size();
}

FileStatus::Status StatusCache::fileStatus(const QFileInfo &fileInfo)
{
    auto filePath = fileInfo.absoluteFilePath();

    if (!mLastDir.isEmpty() && isInDir(mLastDir, filePath)) {
        if (mStatuses.contains(filePath)) {
            return mStatuses.value(filePath);
        } else
            return FileStatus::Unknown;
    }

    if (!addPath(fileInfo.absolutePath()))
        return FileStatus::NoGit;

    if (mStatuses.contains(filePath)) {
        return mStatuses.value(filePath);
    }

    return FileStatus::Unmodified;
}

bool StatusCache::isGitDir(const QString &path)
{
    Git::MiniManager git(path);
    return git.isValid();
}

FileStatus::Status StatusCache::fileStatus(const QString &filePath)
{
    return fileStatus(QFileInfo(filePath));
}

FileStatus::Status StatusCache::pathStatus(const QString &path)
{
    Git::MiniManager git(path);
    if (!git.isValid())
        return FileStatus::NoGit;

    auto statuses = git.repoFilesStatus();
    FileStatus::Status status = FileStatus::Unmodified;

    for (const auto &s : std::as_const(statuses)) {
        const auto filePath = git.path() + QLatin1Char('/') + s.name();

        if (!filePath.startsWith(path)) {
            continue;
        }

        if (status == FileStatus::Unmodified) {
            status = s.status();
        } else if (status != s.status()) {
            return FileStatus::Modified;
        }
    }
    return status;
}
