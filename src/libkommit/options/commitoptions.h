/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QDateTime>
#include <QSharedPointer>

#include <git2/types.h>

#include <Kommit/Branch>

namespace Git
{

class CommitOptionsPrivate;
class LIBKOMMIT_EXPORT CommitOptions
{
public:
    CommitOptions();
    explicit CommitOptions(const CommitOptions &other);
    explicit CommitOptions(CommitOptions &&other) noexcept = default;
    CommitOptions &operator=(const CommitOptions &other) = default;
    CommitOptions &operator=(CommitOptions &&other) noexcept = default;
    ~CommitOptions() = default;

    QString authorName() const;
    void setAuthorName(const QString &authorName);

    QString authorEmail() const;
    void setAuthorEmail(const QString &authorEmail);

    QDateTime authTime() const;
    void setAuthTime(const QDateTime &authTime);

    QString committerName() const;
    void setCommitterName(const QString &committerName);

    QString committerEmail() const;
    void setCommitterEmail(const QString &committerEmail);

    QDateTime commitTime() const;
    void setCommitTime(const QDateTime &commitTime);

    QString reflogMessage() const;
    void setReflogMessage(const QString &reflogMessage);

    Branch branch() const;
    void setBranch(const Branch &branch);

    git_signature *author() const;
    git_signature *committer() const;
    git_commit *parentCommit() const;
    void setRepo(git_repository *repo);

private:

    QSharedPointer<CommitOptionsPrivate> d;

    friend class Repository;
};

}
