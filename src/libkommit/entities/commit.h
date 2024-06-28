/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "gitgraphlane.h"
#include "interfaces.h"
#include "libkommit_export.h"
#include "reference.h"
#include "signature.h"

#include <QDateTime>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include <git2/types.h>

namespace Git
{

class Tree;
class Note;

class LIBKOMMIT_EXPORT Commit : public ITree
{
public:
    enum CommitType { NormalCommit, InitialCommit, ForkCommit, MergeCommit };

    explicit Commit(git_commit *commit);
    ~Commit();

    [[nodiscard]] QSharedPointer<Signature> author() const;
    [[nodiscard]] QSharedPointer<Signature> committer() const;
    [[nodiscard]] const QString &message() const;
    [[nodiscard]] const QString &subject() const;
    [[nodiscard]] const QString &body() const;
    [[nodiscard]] const QString &commitHash() const;
    [[nodiscard]] const QStringList &parents() const;
    [[nodiscard]] const QString &branch() const;
    [[nodiscard]] const QString &extraData() const;
    [[nodiscard]] CommitType type() const;
    [[nodiscard]] const QVector<GraphLane> &lanes() const;
    [[nodiscard]] const QStringList &children() const;
    [[nodiscard]] const QString &commitShortHash() const;
    [[nodiscard]] QDateTime commitTime() const;

    [[nodiscard]] QSharedPointer<Reference> reference() const;

    [[nodiscard]] QSharedPointer<Tree> tree() const override;
    [[nodiscard]] QString treeTitle() const override;

    [[nodiscard]] git_commit *gitCommit() const;

    [[nodiscard]] QSharedPointer<Note> note() const;
    bool createNote(const QString &message);

private:
    git_commit *const mGitCommit;
    QSharedPointer<Signature> mAuthor;
    QSharedPointer<Signature> mCommitter;
    QString mMessage;
    QString mSubject;
    QString mBody;
    QString mCommitHash;
    QString mCommitShortHash;
    QStringList mParentHash;
    QString mBranch;
    QString mExtraData;
    CommitType mType;
    QVector<GraphLane> mLanes;
    QStringList mChildren;
    QSharedPointer<Reference> mReference;
    QDateTime mCommitTime;

    friend class LogList;
    friend class Manager;
    friend class LogsModel;
};

}
