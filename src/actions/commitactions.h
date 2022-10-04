/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"

class CommitActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit CommitActions(Git::Manager *git, QWidget *parent = nullptr);

    Q_REQUIRED_RESULT const QString &commitHash() const;
    void setCommitHash(const QString &newCommitHash);

private Q_SLOTS:
    void browse();
    void checkout();
    void diff();
private:
    DEFINE_ACTION(actionBrowse)
    DEFINE_ACTION(actionCheckout)
    DEFINE_ACTION(actionDiff)
    QString mCommitHash;
};
