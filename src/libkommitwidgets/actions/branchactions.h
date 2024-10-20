/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"
#include "libkommitwidgets_export.h"

#include <Kommit/Branch>

class LIBKOMMITWIDGETS_EXPORT BranchActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit BranchActions(Git::Repository *git, QWidget *parent = nullptr);

    void setBranch(const Git::Branch &newBranch);
    void setOtherBranch(const Git::Branch &newOtherBranch);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void fetch();
    LIBKOMMITWIDGETS_NO_EXPORT void create();
    LIBKOMMITWIDGETS_NO_EXPORT void browse();
    LIBKOMMITWIDGETS_NO_EXPORT void checkout();
    LIBKOMMITWIDGETS_NO_EXPORT void diff();
    LIBKOMMITWIDGETS_NO_EXPORT void remove();
    LIBKOMMITWIDGETS_NO_EXPORT void merge();

    Git::Branch mBranch;
    Git::Branch mOtherBranch;

    DEFINE_ACTION(actionFetch)
    DEFINE_ACTION(actionBrowse)
    DEFINE_ACTION(actionCheckout)
    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionMerge)
    DEFINE_ACTION(actionCreate)
    DEFINE_ACTION(actionRemove)
};
