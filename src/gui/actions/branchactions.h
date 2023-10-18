/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"

namespace Git
{
class Branch;
}
class BranchActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit BranchActions(Git::Manager *git, QWidget *parent = nullptr);

    void setBranchName(const QSharedPointer<Git::Branch> &newBranchName);
    void setOtherBranch(const QSharedPointer<Git::Branch> &newOtherBranch);

private:
    void fetch();
    void create();
    void browse();
    void checkout();
    void diff();
    void remove();
    void merge();
    void note();

    QSharedPointer<Git::Branch> mBranchName;
    QSharedPointer<Git::Branch> mOtherBranch;

    DEFINE_ACTION(actionFetch)
    DEFINE_ACTION(actionBrowse)
    DEFINE_ACTION(actionCheckout)
    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionMerge)
    DEFINE_ACTION(actionCreate)
    DEFINE_ACTION(actionRemove)
    DEFINE_ACTION(actionNote)
};
