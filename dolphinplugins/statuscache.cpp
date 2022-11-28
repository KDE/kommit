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

    const auto statuses = git.repoFilesStatus();

    for (const auto &s : statuses)
        mStatuses.insert(git.path() + QLatin1Char('/') + s.name(), s.status());

    return true;
}

bool StatusCache::isInDir(const QString &dirPath, const QString &filePath)
{
    if (dirPath.endsWith(QLatin1Char('/')))
        return filePath.lastIndexOf(QLatin1Char('/')) == dirPath.size() - 1;
    return filePath.lastIndexOf(QLatin1Char('/')) == dirPath.size();
}

Git::FileStatus::Status StatusCache::fileStatus(const QFileInfo &fileInfo)
{
    const auto filePath = fileInfo.absoluteFilePath();

    if (!mLastDir.isEmpty() && isInDir(mLastDir, filePath)) {
        if (mStatuses.contains(filePath)) {
            return mStatuses.value(filePath);
        } else
            return Git::FileStatus::Unknown;
    }

    if (!addPath(fileInfo.absolutePath()))
        return Git::FileStatus::NoGit;

    if (mStatuses.contains(filePath)) {
        return mStatuses.value(filePath);
    }

    return Git::FileStatus::Unmodified;
}

bool StatusCache::isGitDir(const QString &path)
{
    const Git::MiniManager git(path);
    return git.isValid();
}

Git::FileStatus::Status StatusCache::fileStatus(const QString &filePath)
{
    return fileStatus(QFileInfo(filePath));
}

Git::FileStatus::Status StatusCache::pathStatus(const QString &path)
{
    Git::MiniManager git(path);
    if (!git.isValid())
        return Git::FileStatus::NoGit;

    auto statuses = git.repoFilesStatus();
    Git::FileStatus::Status status = Git::FileStatus::Unmodified;

    for (const auto &s : std::as_const(statuses)) {
        const auto filePath = git.path() + QLatin1Char('/') + s.name();

        if (!filePath.startsWith(path)) {
            continue;
        }

        if (status == Git::FileStatus::Unmodified) {
            status = s.status();
        } else if (status != s.status()) {
            return Git::FileStatus::Modified;
        }
    }
    return status;
}
