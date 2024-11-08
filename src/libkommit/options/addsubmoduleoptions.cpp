/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "addsubmoduleoptions.h"

namespace Git
{

class AddSubmoduleOptionsPrivate
{
public:
    AddSubmoduleOptionsPrivate();
    ~AddSubmoduleOptionsPrivate();

    QString mUrl;
    QString mPath;
    FetchOptions *fetchOptions;
    CheckoutOptions *checkoutOptions;
};

AddSubmoduleOptions::AddSubmoduleOptions()
    : d{new AddSubmoduleOptionsPrivate}
{
}

QString AddSubmoduleOptions::url() const
{
    return d->mUrl;
}

void AddSubmoduleOptions::setUrl(const QString &url)
{
    d->mUrl = url;
}

QString AddSubmoduleOptions::path() const
{
    return d->mPath;
}

void AddSubmoduleOptions::setPath(const QString &path)
{
    d->mPath = path;
}

FetchOptions *AddSubmoduleOptions::fetchOptions() const
{
    return d->fetchOptions;
}

CheckoutOptions *AddSubmoduleOptions::checkoutOptions() const
{
    return d->checkoutOptions;
}

void AddSubmoduleOptions::apply(git_submodule_update_options *opts)
{
    d->checkoutOptions->applyToCheckoutOptions(&opts->checkout_opts);
    d->fetchOptions->apply(&opts->fetch_opts);
}

AddSubmoduleOptionsPrivate::AddSubmoduleOptionsPrivate()
    : fetchOptions{new FetchOptions}
    , checkoutOptions{new CheckoutOptions}
{
}

AddSubmoduleOptionsPrivate::~AddSubmoduleOptionsPrivate()
{
    delete fetchOptions;
    checkoutOptions->deleteLater();
}

}
