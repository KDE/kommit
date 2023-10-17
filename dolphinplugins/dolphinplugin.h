/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "actions.h"

#include <Dolphin/KVersionControlPlugin>

class StatusCache;
class DolphinPlugin : public KVersionControlPlugin
{
    Q_OBJECT

    enum class ChangeStatus {
        Unknown,
        Unmodified,
        Modified,
        Added,
        Removed,
        Renamed,
        Copied,
        UpdatedButInmerged,
        Ignored,
        Untracked,
    };

public:
    explicit DolphinPlugin(QObject *parent, const QList<QVariant> &args);
    ~DolphinPlugin();

    QString fileName() const override;
    bool beginRetrieval(const QString &directory) override;
    void endRetrieval() override;
    ItemVersion itemVersion(const KFileItem &item) const override;
    QList<QAction *> versionControlActions(const KFileItemList &items) const override;
    QList<QAction *> outOfVersionControlActions(const KFileItemList &items) const override;

private Q_SLOTS:
#define f(name, text, args, icon) void name##Clicked();
    ACTIONS_FOR_EACH(f)
#undef f

private:
    StatusCache *mCache;
    QAction *mMainActionGit = nullptr;
    QAction *mMainActionNonGit = nullptr;
    QString mPath;

#define decl(name, text, args, icon) QAction *name;
    ACTIONS_FOR_EACH(decl)
#undef decl
};
