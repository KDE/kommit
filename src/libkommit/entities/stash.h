/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libkommit_export.h"

#include <QDateTime>
#include <QSharedPointer>
#include <QString>

#include <git2/types.h>

namespace Git
{

class Signature;
class Manager;
class Commit;

class LIBKOMMIT_EXPORT Stash
{
public:
    explicit Stash(Git::Manager *git, QString name);
    Stash(size_t index, git_repository *repo, const char *message, const git_oid *stash_id);
    ~Stash();

    Q_REQUIRED_RESULT const QString &message() const;
    Q_REQUIRED_RESULT const QString &subject() const;
    Q_REQUIRED_RESULT const QString &branch() const;
    Q_REQUIRED_RESULT const QDateTime &pushTime() const;

    Q_REQUIRED_RESULT QSharedPointer<Signature> author() const;
    Q_REQUIRED_RESULT QSharedPointer<Signature> committer() const;
    Q_REQUIRED_RESULT QSharedPointer<Commit> commit() const;

    friend class Manager;
    friend class StashesModel;

    Q_REQUIRED_RESULT QString commitHash() const;

private:
    git_commit *ptr{nullptr};
    Git::Manager *mGit = nullptr;

    QSharedPointer<Commit> mCommit;
    QString mMessage;
    QString mCommitHash;
    QString mSubject;
    size_t mIndex;
};

} // namespace Git
