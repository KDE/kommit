/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "filestatus.h"
#include <QMap>

class QFileInfo;
class StatusCache
{
public:
    StatusCache();

    Q_REQUIRED_RESULT Git::FileStatus::Status fileStatus(const QString &path);
    Q_REQUIRED_RESULT Git::FileStatus::Status pathStatus(const QString &path);
    Q_REQUIRED_RESULT Git::FileStatus::Status fileStatus(const QFileInfo &fileInfo);
    Q_REQUIRED_RESULT bool isGitDir(const QString &path);

private:
    Q_REQUIRED_RESULT bool addPath(const QString &path);
    Q_REQUIRED_RESULT bool isInDir(const QString &dirPath, const QString &filePath);

    friend class OverlayTest;

private:
    QString mLastDir;
    QMap<QString, Git::FileStatus::Status> mStatuses;
};
