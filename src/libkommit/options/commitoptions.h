/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QDateTime>
#include <QSharedPointer>

#include <git2/commit.h>
#include <git2/types.h>

#include <Kommit/Branch>
#include <Kommit/Signature>

namespace Git
{

class CommitOptionsPrivate;
class LIBKOMMIT_EXPORT CommitOptions
{
public:
    CommitOptions();

    Branch branch() const;
    void setBranch(const Branch &branch);

    Signature author() const;
    void setAuthor(const Signature &author);

    Signature committer() const;
    void setCommitter(const Signature &committer);

    QString reflogMessage() const;
    void setReflogMessage(const QString &reflogMessage);

    git_commit *parentCommit() const;
    void setRepo(git_repository *repo);

    void apply(git_commit_create_options *options);

private:
    QSharedPointer<CommitOptionsPrivate> d;

    friend class Repository;
};

}
