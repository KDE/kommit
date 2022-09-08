/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "overlayplugin.h"

#include "../src/git/gitmanager.h"
#include <QUrl>
#include <QDebug>
#include <QFileInfo>

OverlayPlugin::OverlayPlugin(QObject *parent) : KOverlayIconPlugin(parent)
{}

QString icon(const FileStatus::Status &status)
{
    switch (status) {
    case FileStatus::Added:
        return QStringLiteral("git-status-added");
    case FileStatus::Ignored:
        return QStringLiteral("git-status-ignored");
    case FileStatus::Modified:
        return QStringLiteral("git-status-modified");
    case FileStatus::Removed:
        return QStringLiteral("git-status-removed");
    case FileStatus::Renamed:
        return QStringLiteral("git-status-renamed");
    case FileStatus::Unknown:
    case FileStatus::Untracked:
        return QStringLiteral("git-status-unknown");
    case FileStatus::Copied:
    case FileStatus::UpdatedButInmerged:
    case FileStatus::Unmodified:
        return QStringLiteral("git-status-update");
    case FileStatus::NoGit:
        return QString();
    default:
        qWarning() << "Unknown icon" << status;
    }
    return QStringLiteral("git-status-update");
}

QStringList OverlayPlugin::getOverlays(const QUrl &url)
{
    if (!url.isLocalFile())
        return {icon(FileStatus::NoGit)};

    const QFileInfo fi(url.toLocalFile());
    if (fi.isDir()) {
        return {icon(mCache.pathStatus(fi.absoluteFilePath()))};
    }

    return {icon(mCache.fileStatus(fi))};
}

#include "overlayplugin.moc"
