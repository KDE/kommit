/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "push.h"

#include <Kommit/Branch>
#include <Kommit/RemoteCallbacks>

namespace Git
{

class PushPrivate {
    PushPrivate(Repository *repository);

    Repository *repo;
    Branch branch;
    Remote remote;
    RemoteCallbacks callbacks;
    int packBuilderPrallelism{1};

    QStringList customHeaders;
    QStringList pushOptions;
};

Push::Push(Repository *repo, QObject *parent)
    : QObject{parent}
    , d{new PushPrivate{repo}}
{
}

int Push::packBuilderPrallelism() const
{
    return d->packBuilderPrallelism;
}

void Push::setPackBuilderPrallelism(const int &packBuilderPrallelism)
{
    d->packBuilderPrallelism = packBuilderPrallelism;
}

Remote Push::remote() const
{
    return d->remote;
}

void Push::setRemote(const Remote &remote)
{
    d->remote = remote;
}

const Git::RemoteCallbacks *Push::remoteCallbacks() const
{
    return &d->callbacks;
}

bool Push::run()
{
    if (!d->remote.connect(Remote::Direction::Push, d->callbacks))
        return false;

    git_push_options opts;
    git_push_options_init(&opts, GIT_PUSH_OPTIONS_VERSION);

    d->callbacks.apply(&opts.callbacks, d->repo);
    opts.pb_parallelism = d->packBuilderPrallelism;

    return GIT_OK == git_remote_push(d->remote.remotePtr(), d->branch.refName().toUtf8().data(), &opts);
}

PushPrivate::PushPrivate(Repository *repository)
    : repo{repository}
{
}

}
