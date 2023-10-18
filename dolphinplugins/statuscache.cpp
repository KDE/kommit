/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "statuscache.h"
#include "qdebug.h"

#include <git2.h>

namespace Impl
{
struct wrapper {
    QMap<QString, KVersionControlPlugin::ItemVersion> files;
    QStringList ignores;
    QString currentPath;
    QString rootPath;
    QString prefix;
    bool allIgnored{false};
};

KVersionControlPlugin::ItemVersion convertToItemVersion(unsigned int status_flags)
{
    KVersionControlPlugin::ItemVersion status;
    if (status_flags & GIT_STATUS_WT_NEW)
        status = KVersionControlPlugin::ItemVersion::AddedVersion;
    else if (status_flags & GIT_STATUS_WT_MODIFIED)
        status = KVersionControlPlugin::ItemVersion::LocallyModifiedVersion;
    else if (status_flags & GIT_STATUS_WT_DELETED)
        status = KVersionControlPlugin::ItemVersion::RemovedVersion;
    else if (status_flags & GIT_STATUS_WT_RENAMED)
        status = KVersionControlPlugin::ItemVersion::ConflictingVersion;
    else if (status_flags & GIT_STATUS_IGNORED)
        status = KVersionControlPlugin::ItemVersion::IgnoredVersion;
    else
        status = KVersionControlPlugin::ItemVersion::UnversionedVersion;

    return status;
}

QString removeSlashAtEnd(const QString &s)
{
    if (s.endsWith("/"))
        return s.mid(0, s.length() - 1);
    return s;
}

auto callback(const char *pa, unsigned int status_flags, void *payload) -> int
{
    auto w = reinterpret_cast<wrapper *>(payload);

    auto path = QString{pa};

    if (w->prefix.startsWith(path) && status_flags & GIT_STATUS_IGNORED) {
        w->allIgnored = true;
        return 1;
    }
    if (!path.startsWith(w->currentPath)) {
        return 0;
    }

    auto status = convertToItemVersion(status_flags);

    QString entryName;
    QString childPath;
    if (path.indexOf("/", w->prefix.length()) == -1) {
        entryName = path.mid(w->prefix.length());
    } else {
        entryName = path.mid(w->prefix.length(), path.indexOf("/", w->prefix.length()) + 1 - w->prefix.length());
        childPath = path.mid(w->prefix.length() + entryName.length());
    }

    qDebug() << path << entryName << childPath;
    if (childPath.isEmpty()) {
        w->files.insert(removeSlashAtEnd(entryName), status);
    } else {
        if (status != KVersionControlPlugin::ItemVersion::IgnoredVersion) {
            //            if (entryName.count('/') == 1 && entryName.endsWith("/"))
            //                w->files.insert(childPath, KVersionControlPlugin::ItemVersion::IgnoredVersion);
            //        } else {
            auto st = w->files.value(entryName, KVersionControlPlugin::ItemVersion::NormalVersion);

            if (st != status) {
                if (st == KVersionControlPlugin::ItemVersion::NormalVersion)
                    w->files.insert(removeSlashAtEnd(entryName), status);
                else
                    w->files.insert(removeSlashAtEnd(entryName), KVersionControlPlugin::ItemVersion::LocallyModifiedVersion);
            }
        }
    }
    return 0;
};
}

StatusCache::StatusCache() = default;

KVersionControlPlugin::ItemVersion StatusCache::status(const QString &name)
{
    if (mCurrentPathIsIgnored || name.startsWith(".git/") || name == ".git")
        return KVersionControlPlugin::IgnoredVersion;

    return mStatuses.value(name, KVersionControlPlugin::ItemVersion::NormalVersion);
}

bool StatusCache::setPath(const QString &path)
{
    if (mPath == path)
        return true;

    mPath = path;

    git_repository *repo;
    int n = git_repository_open_ext(&repo, path.toUtf8().data(), 0, NULL);

    if (n) {
        git_repository_free(repo);
        return false;
    }

    mRepoRootPath = git_repository_workdir(repo);
    if (path.startsWith(mRepoRootPath + ".git/")) {
        mCurrentPathIsIgnored = true;
        return true;
    }

    Impl::wrapper w;
    w.currentPath = path;
    if (!w.currentPath.endsWith("/"))
        w.currentPath.append("/");
    w.rootPath = mRepoRootPath;
    if (!w.rootPath.endsWith("/"))
        w.rootPath.append("/");
    w.prefix = w.currentPath.replace(w.rootPath, "");

    git_status_options opts;
    git_status_options_init(&opts, GIT_STATUS_OPTIONS_VERSION);
    opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    opts.flags = GIT_STATUS_OPT_DEFAULTS;

    //    if (!w.prefix.isEmpty()) {
    //        char *paths[] = {w.prefix.mid(0, w.prefix.length() - 1).toLocal8Bit().data()};
    //        git_strarray array = {paths, 1};
    //        opts.pathspec = array;
    //    }

    git_status_foreach_ext(repo, &opts, Impl::callback, &w);
    mStatuses = w.files;
    mCurrentPathIsIgnored = w.allIgnored;

    git_repository_free(repo);

    return true;
}
