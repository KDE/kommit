/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "checkoutoptions.h"

namespace Git
{

CheckoutOptions::CheckoutOptions()
{
}

void CheckoutOptions::applyToCheckoutOptions(git_checkout_options *opts) const
{
    int strategy{0};

    if (mSafe)
        strategy += GIT_CHECKOUT_SAFE;
    if (mForce)
        strategy += GIT_CHECKOUT_FORCE;
    if (mRecreateMissing)
        strategy += GIT_CHECKOUT_RECREATE_MISSING;
    if (mAllowConflicts)
        strategy += GIT_CHECKOUT_ALLOW_CONFLICTS;
    if (mRemoveUntracked)
        strategy += GIT_CHECKOUT_REMOVE_UNTRACKED;
    if (mRemoveIgnored)
        strategy += GIT_CHECKOUT_REMOVE_IGNORED;
    if (mUpdateOnly)
        strategy += GIT_CHECKOUT_UPDATE_ONLY;
    if (mDontUpdateIndex)
        strategy += GIT_CHECKOUT_DONT_UPDATE_INDEX;
    if (mNoRefresh)
        strategy += GIT_CHECKOUT_NO_REFRESH;
    if (mSkipUnmerged)
        strategy += GIT_CHECKOUT_SKIP_UNMERGED;
    if (mUseOurs)
        strategy += GIT_CHECKOUT_USE_OURS;
    if (mUseTheirs)
        strategy += GIT_CHECKOUT_USE_THEIRS;
    if (mDisablePathspecMatch)
        strategy += GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH;
    if (mSkipLockedDirectories)
        strategy += GIT_CHECKOUT_SKIP_LOCKED_DIRECTORIES;
    if (mDontOverwriteIgnored)
        strategy += GIT_CHECKOUT_DONT_OVERWRITE_IGNORED;
    if (mConflictStyleMerge)
        strategy += GIT_CHECKOUT_CONFLICT_STYLE_MERGE;
    if (mConflictStyleDiFF3)
        strategy += GIT_CHECKOUT_CONFLICT_STYLE_DIFF3;
    if (mDontRemoveExisting)
        strategy += GIT_CHECKOUT_DONT_REMOVE_EXISTING;
    if (mDontWriteIndex)
        strategy += GIT_CHECKOUT_DONT_WRITE_INDEX;
    if (mUpdateSubmodules)
        strategy += GIT_CHECKOUT_UPDATE_SUBMODULES;
    if (mUpdateSubmodulesIfChanged)
        strategy += GIT_CHECKOUT_UPDATE_SUBMODULES_IF_CHANGED;

    opts->checkout_strategy = strategy;

    if (!mAncestorLabel.isEmpty())
        opts->ancestor_label = mAncestorLabel.toLocal8Bit().constData();
    if (!mOurLabel.isEmpty())
        opts->our_label = mOurLabel.toLocal8Bit().constData();
    if (!mTheirLabel.isEmpty())
        opts->their_label = mTheirLabel.toLocal8Bit().constData();
}

} // namespace Git
