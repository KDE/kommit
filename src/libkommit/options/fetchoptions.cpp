/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchoptions.h"
#include "caches/referencecache.h"
#include "certificate.h"
#include "credential.h"
#include "reference.h"
#include "remotecallbacks.h"
#include "repository.h"

namespace Git
{

class FetchOptionsPrivate
{
public:
    explicit FetchOptionsPrivate(FetchOptions *parent, Repository *repo);
    Redirect redirect{Redirect::All};
    int depth{0};
    FetchOptions::UpdateFlags updateFlags;
    RemoteCallbacks *remoteCallbacks;
    FetchOptions::DownloadTags downloadTags{FetchOptions::DownloadTags::Unspecified};
    FetchOptions::Prune prune{FetchOptions::Prune::Unspecified};
    Repository *repo;
    Branch branch;
};

FetchOptions::FetchOptions(Repository *parent)
    : QObject{parent}
    , d{new FetchOptionsPrivate{this, parent}}
{
}

void FetchOptions::apply(git_fetch_options *opts) const
{
    opts->download_tags = static_cast<git_remote_autotag_option_t>(d->downloadTags);
    opts->prune = static_cast<git_fetch_prune_t>(d->prune);
    opts->follow_redirects = static_cast<git_remote_redirect_t>(d->redirect);
    opts->depth = d->depth;
    opts->update_fetchhead = static_cast<unsigned int>(d->updateFlags);

    d->remoteCallbacks->apply(&opts->callbacks, d->repo);
}

Redirect FetchOptions::redirect() const
{
    return d->redirect;
}

void FetchOptions::setRedirect(Redirect redirect)
{
    d->redirect = redirect;
}

int FetchOptions::depth() const
{
    return d->depth;
}

void FetchOptions::setDepth(int depth)
{
    d->depth = depth;
}

FetchOptions::UpdateFlags FetchOptions::updateFlags() const
{
    return d->updateFlags;
}

void FetchOptions::setUpdateFlags(const UpdateFlags &updateFlags)
{
    d->updateFlags = updateFlags;
}

RemoteCallbacks *FetchOptions::remoteCallbacks() const
{
    return d->remoteCallbacks;
}

void FetchOptions::setRemoteCallbacks(RemoteCallbacks *remoteCallbacks)
{
    d->remoteCallbacks = remoteCallbacks;
}

FetchOptions::DownloadTags FetchOptions::downloadTags() const
{
    return d->downloadTags;
}

void FetchOptions::setDownloadTags(DownloadTags downloadTags)
{
    d->downloadTags = downloadTags;
}

FetchOptions::Prune FetchOptions::prune() const
{
    return d->prune;
}

void FetchOptions::setPrune(Prune prune)
{
    d->prune = prune;
}

Branch FetchOptions::branch() const
{
    return d->branch;
}

void FetchOptions::setBranch(const Branch &branch)
{
    d->branch = branch;
}

FetchOptionsPrivate::FetchOptionsPrivate(FetchOptions *parent, Repository *repository)
    : remoteCallbacks{new RemoteCallbacks{parent}}
    , repo{repository}
{
}
}
