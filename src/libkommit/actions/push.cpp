/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "push.h"

#include "remotecallbacks.h"
#include "strarray.h"

#include <git2/remote.h>

namespace Git
{

class PushPrivate
{
public:
    ~PushPrivate();
    Repository *repo;
    Redirect redirect{Redirect::All};
    Remote remote;
    Branch branch;
    RemoteCallbacks *callbacks;
    QStringList customHeaders;
    QStringList remotePushOptions;
    int packBuilderParallelJobs{1};
};

Push::Push(Repository *repo, QObject *parent)
    : AbstractAction{parent}
    , d{new PushPrivate}
{
    d->repo = repo;
    d->callbacks = new RemoteCallbacks{parent};
}

int Push::exec()
{
    if (d->remote.isNull() || d->branch.isNull())
        return GIT_EUSER;

    git_push_options opts = GIT_PUSH_OPTIONS_INIT;
    if (d->callbacks)
        d->callbacks->apply(&opts.callbacks, d->repo);

    opts.follow_redirects = static_cast<git_remote_redirect_t>(d->redirect);

    StrArray customHeaders;
    if (d->customHeaders.size()) {
        customHeaders.fromQStringList(d->customHeaders);
        opts.custom_headers = *customHeaders;
    }

#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) >= QT_VERSION_CHECK(1, 8, 0)
    StrArray remotePushOptions;
    if (d->remotePushOptions.size()) {
        remotePushOptions.fromQStringList(d->remotePushOptions);
        opts.remote_push_options = *remotePushOptions;
    }
#endif

    opts.pb_parallelism = d->packBuilderParallelJobs;
    StrArray a{d->branch.refName()};
    git_remote_push(d->remote.data(), &a, &opts);

    return GIT_OK;
}

Remote Push::remote() const
{
    return d->remote;
}

void Push::setRemote(const Remote &remote)
{
    d->remote = remote;
}

Branch Push::branch() const
{
    return d->branch;
}

void Push::setBranch(const Branch &branch)
{
    d->branch = branch;
}

RemoteCallbacks *Push::remoteCallbacks() const
{
    return d->callbacks;
}

Redirect Push::redirect() const
{
    return d->redirect;
}

void Push::setRedirect(Redirect redirect)
{
    d->redirect = redirect;
}

QStringList Push::customHeaders() const
{
    return d->customHeaders;
}

void Push::setCustomHeaders(const QStringList &customHeaders)
{
    d->customHeaders = customHeaders;
}

QStringList Push::remotePushOptions() const
{
    return d->remotePushOptions;
}

void Push::setRemotePushOptions(const QStringList &remotePushOptions)
{
    d->remotePushOptions = remotePushOptions;
}

int Push::packBuilderParallelJobs() const
{
    return d->packBuilderParallelJobs;
}

void Push::setPackBuilderParallelJobs(int packBuilderParallelJobs)
{
    d->packBuilderParallelJobs = packBuilderParallelJobs;
}

PushPrivate::~PushPrivate()
{
    callbacks->deleteLater();
}

}
