/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "pushoptions.h"

#include "remotecallbacks.h"
#include "strarray.h"
#include "remotecallbacks.h"

namespace Git
{

class PushOptionsPrivate
{
    PushOptions *parent;

public:
    PushOptionsPrivate(PushOptions *parent, Repository *repo);
    ~PushOptionsPrivate();

    Repository *repo;
    Redirect redirect{Redirect::All};
    Remote remote;
    Branch branch;
    RemoteCallbacks *callbacks;
    QStringList customHeaders;
    QStringList remotePushOptions;
    int packBuilderParallelJobs{1};
};

PushOptionsPrivate::PushOptionsPrivate(PushOptions *parent, Repository *repo)
    : parent{parent}
    , repo{repo}
    , callbacks{new RemoteCallbacks}
{
}

PushOptionsPrivate::~PushOptionsPrivate()
{
    callbacks->deleteLater();
}

PushOptions::PushOptions(Repository *repo)
    : d{new PushOptionsPrivate{this, repo}}
{
}

PushOptions::~PushOptions()
{
}

void PushOptions::apply(git_push_options *opts)
{
    if (d->remote.isNull() || d->branch.isNull())
        return;

    d->callbacks->apply(&opts->callbacks, d->repo);

    opts->follow_redirects = static_cast<git_remote_redirect_t>(d->redirect);

    StrArray customHeaders;
    if (d->customHeaders.size()) {
        customHeaders.fromQStringList(d->customHeaders);
        opts->custom_headers = *customHeaders;
    }

    StrArray remotePushOptions;
    if (d->remotePushOptions.size()) {
        remotePushOptions.fromQStringList(d->remotePushOptions);
        opts->remote_push_options = *remotePushOptions;
    }

    opts->pb_parallelism = d->packBuilderParallelJobs;
}

Remote PushOptions::remote() const
{
    return d->remote;
}

void PushOptions::setRemote(const Remote &remote)
{
    d->remote = remote;
}

Branch PushOptions::branch() const
{
    return d->branch;
}

void PushOptions::setBranch(const Branch &branch)
{
    d->branch = branch;
}

RemoteCallbacks *PushOptions::remoteCallbacks() const
{
    return d->callbacks;
}

Redirect PushOptions::redirect() const
{
    return d->redirect;
}

void PushOptions::setRedirect(Redirect redirect)
{
    d->redirect = redirect;
}

QStringList PushOptions::customHeaders() const
{
    return d->customHeaders;
}

void PushOptions::setCustomHeaders(const QStringList &customHeaders)
{
    d->customHeaders = customHeaders;
}

QStringList PushOptions::remotePushOptions() const
{
    return d->remotePushOptions;
}

void PushOptions::setRemotePushOptions(const QStringList &remotePushOptions)
{
    d->remotePushOptions = remotePushOptions;
}

int PushOptions::packBuilderParallelJobs() const
{
    return d->packBuilderParallelJobs;
}

void PushOptions::setPackBuilderParallelJobs(int packBuilderParallelJobs)
{
    d->packBuilderParallelJobs = packBuilderParallelJobs;
}

}
