/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"
#include "libkommitwidgets_export.h"

namespace Git
{
class Stash;
};

class LIBKOMMITWIDGETS_EXPORT StashActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit StashActions(Git::Manager *git, QWidget *parent = nullptr);

    Q_REQUIRED_RESULT QSharedPointer<Git::Stash> stash() const;
    void setStash(QSharedPointer<Git::Stash> stash);

public Q_SLOTS:
    void apply();
    void drop();
    void pop();
    void diff();
    void create();

private:
    QSharedPointer<Git::Stash> mStash;

    DEFINE_ACTION(actionPop)
    DEFINE_ACTION(actionApply)
    DEFINE_ACTION(actionDrop)
    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionNew)
};
