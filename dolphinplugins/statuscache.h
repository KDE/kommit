/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMap>
#include <QString>

#include <Dolphin/KVersionControlPlugin>
namespace Impl
{
QString removeSlashAtEnd(const QString &s);
};

class QFileInfo;
class StatusCache
{
public:
    StatusCache();

    Q_REQUIRED_RESULT KVersionControlPlugin::ItemVersion status(const QString &name);
    bool setPath(const QString &path);

private:
    QString mRepoRootPath;
    QString mPath;
    QMap<QString, KVersionControlPlugin::ItemVersion> mStatuses;
    bool mCurrentPathIsIgnored{false};

    friend class OverlayTest;
};
