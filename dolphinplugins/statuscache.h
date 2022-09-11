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

    FileStatus::Status fileStatus(const QString &path);
    FileStatus::Status pathStatus(const QString &path);
    FileStatus::Status fileStatus(const QFileInfo &fileInfo);
    bool isGitDir(const QString &path);

private:
    bool addPath(const QString &path);
    bool isInDir(const QString &dirPath, const QString &filePath);

    friend class OverlayTest;

private:
    QString mLastDir;
    QMap<QString, FileStatus::Status> mStatuses;
};
