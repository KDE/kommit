/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMap>
#include <QString>

#include <Dolphin/KVersionControlPlugin>
#include <git2/types.h>
namespace Impl
{
Q_REQUIRED_RESULT QString removeSlashAtEnd(const QString &s);
}

class QFileInfo;
class StatusCache
{
public:
    StatusCache();

    Q_REQUIRED_RESULT KVersionControlPlugin::ItemVersion status(const QString &name);
    bool setPath(const QString &path);
    Q_REQUIRED_RESULT QString currentBranch() const;
    Q_REQUIRED_RESULT QStringList submodules() const;

    Q_REQUIRED_RESULT QString repoRootPath() const;

    Q_REQUIRED_RESULT QString submoduleName() const;

private:
    QString mRepoRootPath;
    QString mPath;
    QString mSubmoduleName;

    QMap<QString, KVersionControlPlugin::ItemVersion> mStatuses;
    bool mCurrentPathIsIgnored{false};

    git_repository *mRepo{nullptr};

    friend class OverlayTest;
};
