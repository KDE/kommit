/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/checkout.h>

#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT CheckoutOptions
{
public:
    //    enum CheckoutStrategy {
    //        None = 0,
    //        Safe = (1u << 0),
    //        Force = (1u << 1),
    //        RecreateMissing = (1u << 2),
    //        AllowConflicts = (1u << 4),
    //        RemoveUntracked = (1u << 5),
    //        RemoveIgnored = (1u << 6),
    //        UpdateOnly = (1u << 7),
    //        DontUpdateIndex = (1u << 8),
    //        NoRefresh = (1u << 9),
    //        SkipUnmerged = (1u << 10),
    //        UseOurs = (1u << 11),
    //        UseTheirs = (1u << 12),
    //        DisablePathspecMatch = (1u << 13),
    //        SkipLockedDirectories = (1u << 18),
    //        DontOverwriteIgnored = (1u << 19),
    //        ConflictStyleMerge = (1u << 20),
    //        ConflictStyleDiFF3 = (1u << 21),
    //        DontRemoveExisting = (1u << 22),
    //        DontWriteIndex = (1u << 23),
    //        UpdateSubmodules = (1u << 16),
    //        UpdateSubmodulesIfChanged = (1u << 17),
    //    };

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
