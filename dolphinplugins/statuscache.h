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
[[nodiscard]] QString removeSlashAtEnd(const QString &s);
}

class QFileInfo;
class StatusCache
{
public:
    StatusCache();

    [[nodiscard]] KVersionControlPlugin::ItemVersion status(const QString &name);
    [[nodiscard]] bool setPath(const QString &path);
    [[nodiscard]] QString currentBranch() const;
    [[nodiscard]] QStringList submodules() const;

    [[nodiscard]] QString repoRootPath() const;

    [[nodiscard]] QString submoduleName() const;

private:
    QString mRepoRootPath;
    QString mPath;
    QString mSubmoduleName;

    QMap<QString, KVersionControlPlugin::ItemVersion> mStatuses;
    bool mCurrentPathIsIgnored{false};

    git_repository *mRepo{nullptr};

    friend class OverlayTest;
};
