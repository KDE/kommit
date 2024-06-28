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

    [[nodiscard]] const QString &message() const;
    [[nodiscard]] const QString &subject() const;
    [[nodiscard]] const QString &branch() const;
    [[nodiscard]] const QDateTime &pushTime() const;

    [[nodiscard]] QSharedPointer<Signature> author() const;
    [[nodiscard]] QSharedPointer<Signature> committer() const;
    [[nodiscard]] QSharedPointer<Commit> commit() const;

    friend class Manager;
    friend class StashesModel;

    [[nodiscard]] QString commitHash() const;

    [[nodiscard]] size_t index() const;

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
