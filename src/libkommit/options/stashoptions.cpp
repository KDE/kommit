/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashoptions.h"

#include "checkoutoptions.h"

namespace Git
{

class StashOptionsPrivate
{
    StashOptions *parent;

public:
    explicit StashOptionsPrivate(StashOptions *parent);
    ~StashOptionsPrivate();

    StashOptions::StashApplyFlags flags;
    CheckoutOptions *checkoutOptions;
};

StashOptionsPrivate::StashOptionsPrivate(StashOptions *parent)
    : parent{parent}
    , checkoutOptions{new CheckoutOptions}
{
}

StashOptionsPrivate::~StashOptionsPrivate()
{
    checkoutOptions->deleteLater();
}

StashOptions::StashOptions()
    : d{new StashOptionsPrivate{this}}
{
}

void StashOptions::apply(git_stash_apply_options *opts)
{
    d->checkoutOptions->apply(&opts->checkout_options);
    opts->flags = static_cast<uint32_t>(d->flags);
}

StashOptions::StashApplyFlags StashOptions::flags() const
{
    return d->flags;
}

void StashOptions::setFlags(StashApplyFlags flags)
{
    d->flags = flags;
}

CheckoutOptions *StashOptions::checkoutOptions() const
{
    return d->checkoutOptions;
}

}
