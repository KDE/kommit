/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "overlayplugin.h"
#include "kommit_dolphinplugins_debug.h"

#include <QFileInfo>
#include <QUrl>

OverlayPlugin::OverlayPlugin(QObject *parent)
    : KOverlayIconPlugin(parent)
{
}

// TODO: Use Git::statusIcon
QString icon(const Git::FileStatus::Status &status)
{
    switch (status) {
    case Git::FileStatus::Added:
        return QStringLiteral("git-status-added");
    case Git::FileStatus::Ignored:
        return QStringLiteral("git-status-ignored");
    case Git::FileStatus::Modified:
        return QStringLiteral("git-status-modified");
    case Git::FileStatus::Removed:
        return QStringLiteral("git-status-removed");
    case Git::FileStatus::Renamed:
        return QStringLiteral("git-status-renamed");
    case Git::FileStatus::Unknown:
    case Git::FileStatus::Untracked:
        return QStringLiteral("git-status-unknown");
    case Git::FileStatus::Copied:
    case Git::FileStatus::UpdatedButInmerged:
    case Git::FileStatus::Unmodified:
        return QStringLiteral("git-status-update");
    case Git::FileStatus::NoGit:
        return {};
    default:
        qCWarning(KOMMIT_DOLPHIN_PLUGINS_LOG) << "Unknown icon" << status;
    }
    return QStringLiteral("git-status-update");
}

QStringList OverlayPlugin::getOverlays(const QUrl &url)
{
    if (!url.isLocalFile())
        return {icon(Git::FileStatus::NoGit)};

    const QFileInfo fi(url.toLocalFile());
    if (fi.isDir()) {
        return {icon(mCache.pathStatus(fi.absoluteFilePath()))};
    }

    return {icon(mCache.fileStatus(fi))};
}

#include "overlayplugin.moc"

#include "moc_overlayplugin.cpp"
