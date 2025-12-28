/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "cloneoptions.h"

#include "checkoutoptions.h"
#include "fetchoptions.h"
#include "repository.h"

namespace Git
{

class CloneOptionsPrivate
{
    CloneOptions *parent;

public:
    CloneOptionsPrivate(CloneOptions *parent, Repository *repo);
    ~CloneOptionsPrivate();

    QString url;
    QString localPath;
    QString checkoutBranchName;

    FetchOptions *fetchOptions;
    CheckoutOptions *checkoutOptions;
};

CloneOptionsPrivate::CloneOptionsPrivate(CloneOptions *parent, Repository *repo)
    : parent{parent}
    , fetchOptions{new FetchOptions{repo}}
    , checkoutOptions{new CheckoutOptions}
{
}

CloneOptionsPrivate::~CloneOptionsPrivate()
{
    delete fetchOptions;
    checkoutOptions->deleteLater();
}

CloneOptions::CloneOptions(Repository *repo)
    : d{new CloneOptionsPrivate{this, repo}}
{
}

CloneOptions::~CloneOptions()
{
}

void CloneOptions::apply(git_clone_options *opts)
{
    d->checkoutOptions->apply(&opts->checkout_opts);
    d->fetchOptions->apply(&opts->fetch_opts);

    opts->checkout_branch = d->checkoutBranchName.toUtf8().data();
}

QString CloneOptions::url() const
{
    return d->url;
}

void CloneOptions::setUrl(const QString &url)
{
    d->url = url;
}

QString CloneOptions::localPath() const
{
    return d->localPath;
}

void CloneOptions::setLocalPath(const QString &localPath)
{
    d->localPath = localPath;
}

QString CloneOptions::checkoutBranchName() const
{
    return d->checkoutBranchName;
}

void CloneOptions::setCheckoutBranchName(const QString &checkoutBranchName)
{
    d->checkoutBranchName = checkoutBranchName;
}

CheckoutOptions *CloneOptions::checkoutOptions() const
{
    return d->checkoutOptions;
}

FetchOptions *CloneOptions::fetchOptions() const
{
    return d->fetchOptions;
}

}
