/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#define PACK(...) {__VA_ARGS__}

// clang-format off
#define ACTIONS_FOR_EACH(f)                                                                                                                                    \
    f(actionClone,          i18n("Clone"),              PACK(QStringLiteral("clone"), PACK(mPath)),         QLatin1String())                                   \
    f(actionInit,           i18n("Init"),               PACK(QStringLiteral("init"), PACK(mPath)),          QLatin1String())                                   \
    f(actionOpen,           i18n("Open"),               PACK(mPath),                                        QLatin1String())                                   \
    f(actionPull,           i18n("Pull"),               PACK(QStringLiteral("pull"), mPath),                QStringLiteral("git-pull"))                        \
    f(actionFetch,          i18n("Fetch"),              PACK(QStringLiteral("fetch"), mPath),               QStringLiteral("git-fetch"))                       \
    f(actionPush,           i18n("Push"),               PACK(QStringLiteral("push"), mPath),                QStringLiteral("git-push"))                        \
    f(actionMerge,          i18n("Merge"),              PACK(QStringLiteral("merge"), mPath),               QStringLiteral("git-merge"))                       \
    f(actionSwitchCheckout, i18n("Switch/Checkout"),    PACK(QStringLiteral("switch-checkout"), mPath),     QLatin1String())                                   \
    f(actionModifications,  i18n("Modifications"),      PACK(QStringLiteral("changes"), mPath),             QStringLiteral("kommit-changedfiles"))             \
    f(actionDiff,           i18n("Diff"),               PACK(QStringLiteral("diff"), mPath),                QLatin1String())                                   \
    f(actionIgnoreFile,     i18n("Ignore file"),        PACK(QStringLiteral("ignore"), mPath),              QStringLiteral("git-ignore"))                      \
    f(actionHistory,        i18n("History"),            PACK(QStringLiteral("history"), mPath),             QLatin1String())                                   \
    f(actionBlame,          i18n("Blame"),              PACK(QStringLiteral("blame"), mPath),               QLatin1String())                                   \
    f(actionAdd,            i18n("Add"),                PACK(QStringLiteral("add"), mPath),                 QLatin1String())                                   \
    f(actionRemove,         i18n("Remove"),             PACK(QStringLiteral("remove"), mPath),              QLatin1String())                                   \
    f(actionCreateTag,      i18n("Create tag"),         PACK(QStringLiteral("create-tag"), mPath),          QLatin1String())                                   \
    f(actionCleanup,        i18n("Cleanup"),            PACK(QStringLiteral("cleanup"), mPath),             QLatin1String())                                   \
    f(actionDiffBranches,   i18n("Diff branches"),      PACK(QStringLiteral("diff_branches"), mPath),       QLatin1String())

// clang-format on
