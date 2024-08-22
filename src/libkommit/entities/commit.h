/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

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
class CommitPrivate;

class LIBKOMMIT_EXPORT Commit : public ITree, public IOid
{
public:
    enum CommitType { NormalCommit, InitialCommit, ForkCommit, MergeCommit };

    explicit Commit(git_commit *commit);
    ~Commit();

    [[nodiscard]] QSharedPointer<Signature> author();
    [[nodiscard]] QSharedPointer<Signature> committer();
    [[nodiscard]] QString message() const;
    [[nodiscard]] QString body() const;
    [[nodiscard]] const QString &commitHash() const;
    [[nodiscard]] const QStringList &parents() const;
    [[nodiscard]] QSharedPointer<Branch> branch() const;
    [[nodiscard]] const QStringList &children() const;
    [[nodiscard]] const QString &commitShortHash() const;
    [[nodiscard]] QDateTime commitTime() const;

    [[nodiscard]] QList<QSharedPointer<Reference>> references() const;

    [[nodiscard]] QSharedPointer<Tree> tree() const override;
    [[nodiscard]] QString treeTitle() const override;

    [[nodiscard]] git_commit *gitCommit() const;

    [[nodiscard]] QSharedPointer<Note> note();
    bool createNote(const QString &message);
    QSharedPointer<Oid> oid() const override;

private:
    CommitPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Commit);

    QString mCommitHash;
    QString mCommitShortHash;

    void clearChildren();
    void setReferences(QList<QSharedPointer<Reference>> refs);
    void addChild(const QString &childHash);

    friend class LogList;
    friend class Manager;
    friend class CommitsCache;
};
}
