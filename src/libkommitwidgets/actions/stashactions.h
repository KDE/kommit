/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"
#include "libkommitwidgets_export.h"

#include <Kommit/Stash>

class LIBKOMMITWIDGETS_EXPORT StashActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit StashActions(Git::Repository *git, QWidget *parent = nullptr);

    [[nodiscard]] const Git::Stash &stash() const;
    void setStash(const Git::Stash &stash);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void apply();
    LIBKOMMITWIDGETS_NO_EXPORT void drop();
    LIBKOMMITWIDGETS_NO_EXPORT void pop();
    LIBKOMMITWIDGETS_NO_EXPORT void diff();
    LIBKOMMITWIDGETS_NO_EXPORT void create();
    Git::Stash mStash;

    DEFINE_ACTION(actionPop)
    DEFINE_ACTION(actionApply)
    DEFINE_ACTION(actionDrop)
    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionNew)
};
