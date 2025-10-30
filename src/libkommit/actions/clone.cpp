/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "clone.h"

#include "actions/fetch.h"
#include "remotecallbacks.h"

#include <git2/clone.h>

namespace Git
{

class ClonePrivate : public QSharedData
{
    Clone *q_ptr;
    Q_DECLARE_PUBLIC(Clone)

public:
    ClonePrivate(Clone *parent, Repository *repo);

    QString url;
    QString localPath;
    FileMode fileMode;
    FileMode dirMode;
    RemoteCallbacks callbacks;
    int depth{0};
    QString checkoutBranchName;
    Fetch fetch;
    Repository *repo;
};

ClonePrivate::ClonePrivate(Clone *parent, Repository *repo)
    : q_ptr{parent}
    , fetch{repo}
    , repo{repo}
{
}

Clone::Clone(Repository *repo, QObject *parent)
    : AbstractAction{parent}
    , d{new ClonePrivate{this, repo}}
{
}

int Clone::exec()
{
    git_clone_options opts;
    git_clone_options_init(&opts, GIT_CLONE_OPTIONS_VERSION);

    opts.checkout_branch = d->checkoutBranchName.toUtf8().data();

    if (d->dirMode.isValid())
        opts.checkout_opts.dir_mode = d->dirMode.value();
    if (d->fileMode.isValid())
        opts.checkout_opts.file_mode = d->fileMode.value();

    opts.fetch_opts.depth = d->depth;

    git_repository *repo;
    auto n = git_clone(&repo, d->url.toStdString().c_str(), d->localPath.toStdString().c_str(), &opts);
    return 0;
}

QString Clone::url() const
{
    return d->url;
}

void Clone::setUrl(const QString &url)
{
    d->url = url;
}

QString Clone::localPath() const
{
    return d->localPath;
}

void Clone::setLocalPath(const QString &localPath)
{
    d->localPath = localPath;
}

FileMode Clone::fileMode() const
{
    return d->fileMode;
}

void Clone::setFileMode(const FileMode &fileMode)
{
    d->fileMode = fileMode;
}

FileMode Clone::dirMode() const
{
    return d->dirMode;
}

void Clone::setDirMode(const FileMode &dirMode)
{
    d->dirMode = dirMode;
}

int Clone::depth() const
{
    return d->depth;
}

void Clone::setDepth(const int &depth)
{
    d->depth = depth;
}

QString Clone::checkoutBranchName() const
{
    return d->checkoutBranchName;
}

void Clone::setCheckoutBranchName(const QString &checkoutBranchName)
{
    d->checkoutBranchName = checkoutBranchName;
}

Fetch *Clone::fetch()
{
    return &d->fetch;
}
}
