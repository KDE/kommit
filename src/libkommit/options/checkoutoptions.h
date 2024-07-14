/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include "qobjectdefs.h"

#include <git2/checkout.h>

#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT CheckoutOptions
{
public:
    enum class CheckoutStrategy {
        None = 0,
        Safe = GIT_CHECKOUT_SAFE,
        Force = GIT_CHECKOUT_FORCE,
        RecreateMissing = GIT_CHECKOUT_RECREATE_MISSING,
        AllowConflicts = GIT_CHECKOUT_ALLOW_CONFLICTS,
        RemoveUntracked = GIT_CHECKOUT_REMOVE_UNTRACKED,
        RemoveIgnored = GIT_CHECKOUT_REMOVE_IGNORED,
        UpdateOnly = GIT_CHECKOUT_UPDATE_ONLY,
        DontUpdateIndex = GIT_CHECKOUT_DONT_UPDATE_INDEX,
        NoRefresh = GIT_CHECKOUT_NO_REFRESH,
        SkipUnmerged = GIT_CHECKOUT_SKIP_UNMERGED,
        UseOurs = GIT_CHECKOUT_USE_OURS,
        UseTheirs = GIT_CHECKOUT_USE_THEIRS,
        DisablePathspecMatch = GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH,
        SkipLockedDirectories = GIT_CHECKOUT_SKIP_LOCKED_DIRECTORIES,
        DontOverwriteIgnored = GIT_CHECKOUT_DONT_OVERWRITE_IGNORED,
        ConflictStyleMerge = GIT_CHECKOUT_CONFLICT_STYLE_MERGE,
        ConflictStyleDiFF3 = GIT_CHECKOUT_CONFLICT_STYLE_DIFF3,
        DontRemoveExisting = GIT_CHECKOUT_DONT_REMOVE_EXISTING,
        DontWriteIndex = GIT_CHECKOUT_DONT_WRITE_INDEX,
        UpdateSubmodules = GIT_CHECKOUT_UPDATE_SUBMODULES,
        UpdateSubmodulesIfChanged = GIT_CHECKOUT_UPDATE_SUBMODULES_IF_CHANGED,
    };

    // Q_DECLARE_FLAGS(CheckoutStrategies, CheckoutStrategy)
    // Q_FLAG(CheckoutStrategies);

    bool mSafe{false};
    bool mForce{false};
    bool mRecreateMissing{false};
    bool mAllowConflicts{false};
    bool mRemoveUntracked{false};
    bool mRemoveIgnored{false};
    bool mUpdateOnly{false};
    bool mDontUpdateIndex{false};
    bool mNoRefresh{false};
    bool mSkipUnmerged{false};
    bool mUseOurs{false};
    bool mUseTheirs{false};
    bool mDisablePathspecMatch{false};
    bool mSkipLockedDirectories{false};
    bool mDontOverwriteIgnored{false};
    bool mConflictStyleMerge{false};
    bool mConflictStyleDiFF3{false};
    bool mDontRemoveExisting{false};
    bool mDontWriteIndex{false};
    bool mUpdateSubmodules{false};
    bool mUpdateSubmodulesIfChanged{false};

    QString mAncestorLabel;
    QString mOurLabel;
    QString mTheirLabel;

    CheckoutOptions();

    void applyToCheckoutOptions(git_checkout_options *opts) const;
};

} // namespace Git
