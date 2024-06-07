/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "statuscache.h"
#include "qdebug.h"

#include <QDir>
#include <git2.h>

#define BEGIN int err = 0;
#define STEP err = err ? err:
#define PRINT_ERROR                                                                                                                                            \
    do {                                                                                                                                                       \
        if (err) {                                                                                                                                             \
            const git_error *lg2err;                                                                                                                           \
            if ((lg2err = git_error_last()) != NULL && lg2err->message != NULL) {                                                                              \
                auto msg = QString{lg2err->message};                                                                                                           \
                qDebug() << "Error" << Q_FUNC_INFO << err << ":" << msg;                                                                                       \
            }                                                                                                                                                  \
        }                                                                                                                                                      \
    } while (false)

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
    if (s.endsWith(QLatin1Char('/')))
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
    if (path.indexOf(QLatin1Char('/'), w->prefix.length()) == -1) {
        entryName = path.mid(w->prefix.length());
    } else {
        entryName = path.mid(w->prefix.length(), path.indexOf(QLatin1Char('/'), w->prefix.length()) + 1 - w->prefix.length());
        childPath = path.mid(w->prefix.length() + entryName.length());
    }

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

QString findParentContansGit(const QString &dir)
{
    auto path = removeSlashAtEnd(dir);

    QDir d;
    while (path.contains(QLatin1Char('/'))) {
        if (d.exists(path + QStringLiteral("/.git/")))
            return path;

        path = path.mid(0, path.lastIndexOf(QLatin1Char('/')));
    }

    return {};
}

QStringList submodules(git_repository *repo)
{
    if (!repo)
        return {};

    struct Data {
        QStringList list;
        git_repository *repo;
    };

    auto cb = [](git_submodule *sm, const char *name, void *payload) -> int {
        Q_UNUSED(sm);
        auto data = reinterpret_cast<Data *>(payload);
        data->list << name;

        return 0;
    };

    Data data;
    data.repo = repo;
    git_submodule_foreach(repo, cb, &data);

    return data.list;
}

QString submodulePath(const QString &rootPath, const QString &childPath)
{
    qDebug() << Q_FUNC_INFO << rootPath << childPath;
    git_repository *repo;
    auto n = git_repository_open_ext(&repo, rootPath.toUtf8().data(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);

    if (n) {
        git_repository_free(repo);
        return {};
    }

    auto submodulesList = submodules(repo);
    for (auto const &submodule : submodulesList) {
        qDebug() << "FFFFF" << submodule << rootPath << childPath;
        if (rootPath + "/" + submodule + "/" == childPath)
            return submodule;
    }
    return {};
}
}

StatusCache::StatusCache() = default;

KVersionControlPlugin::ItemVersion StatusCache::status(const QString &name)
{
    if (mCurrentPathIsIgnored || name.startsWith(QStringLiteral(".git/")) || name == QStringLiteral(".git"))
        return KVersionControlPlugin::IgnoredVersion;

    return mStatuses.value(name, KVersionControlPlugin::ItemVersion::NormalVersion);
}

bool StatusCache::setPath(const QString &path)
{
    if (mPath == path)
        return true;

    mPath = path;

    if (mRepo)
        git_repository_free(mRepo);

    int n = git_repository_open_ext(&mRepo, path.toUtf8().data(), 0, NULL);

    if (n) {
        git_repository_free(mRepo);
        mRepo = nullptr;
        return false;
    }

    auto rootPath = Impl::findParentContansGit(path);
    mSubmoduleName = Impl::submodulePath(rootPath, path);
    qDebug() << Q_FUNC_INFO << mSubmoduleName;

    mRepoRootPath = git_repository_workdir(mRepo);
    if (path.startsWith(mRepoRootPath + QStringLiteral(".git/"))) {
        mCurrentPathIsIgnored = true;

        git_repository_free(mRepo);
        return true;
    }

    Impl::wrapper w;
    w.currentPath = path;
    if (!w.currentPath.endsWith(QLatin1Char('/')))
        w.currentPath.append(QLatin1Char('/'));
    w.rootPath = mRepoRootPath;
    if (!w.rootPath.endsWith(QLatin1Char('/')))
        w.rootPath.append(QLatin1Char('/'));
    w.prefix = w.currentPath.remove(w.rootPath);

    git_status_options opts;
    git_status_options_init(&opts, GIT_STATUS_OPTIONS_VERSION);
    opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    opts.flags = GIT_STATUS_OPT_DEFAULTS;

    //    if (!w.prefix.isEmpty()) {
    //        char *paths[] = {w.prefix.mid(0, w.prefix.length() - 1).toLocal8Bit().data()};
    //        git_strarray array = {paths, 1};
    //        opts.pathspec = array;
    //    }

    git_status_foreach_ext(mRepo, &opts, Impl::callback, &w);
    mStatuses = w.files;
    mCurrentPathIsIgnored = w.allIgnored;

    return true;
}

QString StatusCache::currentBranch() const
{
    if (git_repository_head_detached(mRepo) == 1)
        return {};

    git_reference *ref;
    BEGIN
    STEP git_repository_head(&ref, mRepo);
    if (err) {
        PRINT_ERROR;
        return {};
    }

    QString branchName{git_reference_shorthand(ref)};

    git_reference_free(ref);

    return branchName;
}

QStringList StatusCache::submodules() const
{
    if (!mRepo)
        return {};

    struct Data {
        QStringList list;
        git_repository *repo;
    };

    auto cb = [](git_submodule *sm, const char *name, void *payload) -> int {
        Q_UNUSED(sm);
        qDebug() << "Submodule name" << name;
        auto data = reinterpret_cast<Data *>(payload);
        // data->list.append(git_submodule_path(sm));
        data->list << name;

        return 0;
    };

    Data data;
    data.repo = mRepo;
    git_submodule_foreach(mRepo, cb, &data);

    return data.list;
}

QString StatusCache::repoRootPath() const
{
    return mRepoRootPath;
}

QString StatusCache::submoduleName() const
{
    return mSubmoduleName;
}
