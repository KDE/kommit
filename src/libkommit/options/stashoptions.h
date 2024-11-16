/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>

#include <git2/stash.h>

namespace Git
{

class CheckoutOptions;
class StashOptionsPrivate;
class StashOptions
{
    Q_GADGET

public:
    StashOptions();

    enum class StashApplyFlag {
        Default = GIT_STASH_APPLY_DEFAULT,
        ReinstateIndex = GIT_STASH_APPLY_REINSTATE_INDEX
    };
    Q_DECLARE_FLAGS(StashApplyFlags, StashApplyFlag)
    Q_FLAG(StashApplyFlags)

    void apply(git_stash_apply_options *opts);

    [[nodiscard]] StashApplyFlags flags() const;
    void setFlags(StashApplyFlags flags);

    [[nodiscard]] CheckoutOptions *checkoutOptions() const;

private:
    QSharedPointer<StashOptionsPrivate> d;
};

}
