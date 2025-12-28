/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include "qobjectdefs.h"

#include <Kommit/FileDelta>
#include <Kommit/FileMode>
#include <diffdelta.h>

#include <git2/checkout.h>

#include <QObject>
#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT CheckoutOptions : public QObject
{
    Q_OBJECT

public:
    enum class CheckoutStrategy {
        None = GIT_CHECKOUT_NONE,
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
    Q_DECLARE_FLAGS(CheckoutStrategies, CheckoutStrategy)
    Q_FLAG(CheckoutStrategies)

    enum class NotifyFlag {
        None = GIT_CHECKOUT_NOTIFY_NONE,
        Conflict = GIT_CHECKOUT_NOTIFY_CONFLICT,
        Dirty = GIT_CHECKOUT_NOTIFY_DIRTY,
        Untracked = GIT_CHECKOUT_NOTIFY_UNTRACKED,
        Ignored = GIT_CHECKOUT_NOTIFY_IGNORED,
        All = GIT_CHECKOUT_NOTIFY_ALL,
    };
    Q_DECLARE_FLAGS(NotifyFlags, NotifyFlag)
    Q_FLAG(NotifyFlags)

    CheckoutOptions(QObject *parent = nullptr);

    void apply(git_checkout_options *opts);

Q_SIGNALS:
    void checkoutNotify(NotifyFlag notify, QString path, DiffFile baseline, DiffFile target, DiffFile workdir);
    void checkoutProgress(QString path, size_t completed_steps, size_t total_steps);
    void checkoutPerfData(size_t mkdir_calls, size_t stat_calls, size_t chmod_calls);

private:
    QString mAncestorLabel;
    QString mOurLabel;
    QString mTheirLabel;
    FileMode mDirMode;
    FileMode mFileMode;
    NotifyFlags mNotifyFlags{NotifyFlag::All};
    CheckoutStrategies mCheckoutStrategies{CheckoutStrategy::Safe};
};

} // namespace Git
