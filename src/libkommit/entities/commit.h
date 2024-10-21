/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include "reference.h"
#include "signature.h"

#include <QDateTime>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include <git2/types.h>

#include <Kommit/IOid>
#include <Kommit/ITree>
#include <Kommit/Oid>

namespace Git
{

class Tree;
class Note;

class CommitPrivate;
class LIBKOMMIT_EXPORT Commit : public ITree, public IOid
{
public:
    enum CommitType { NormalCommit, InitialCommit, ForkCommit, MergeCommit };

    Commit();
    explicit Commit(git_commit *commit);
    Commit(const Commit &other);
    Commit &operator=(const Commit &other);
    bool operator==(const Commit &other) const;
    bool operator!=(const Commit &other) const;
    bool operator<(const Commit &other) const;

    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_commit *data() const;
    [[nodiscard]] const git_commit *constData() const;

    [[nodiscard]] const Signature &author() const;
    [[nodiscard]] const Signature &committer() const;
    [[nodiscard]] QString message() const;
    [[nodiscard]] QString body() const;
    [[nodiscard]] QString summary() const;
    [[nodiscard]] const QString &commitHash() const;
    [[nodiscard]] const QStringList &parents() const;
    [[nodiscard]] const Branch &branch() const;
    [[nodiscard]] const QStringList &children() const;
    [[nodiscard]] const QString &commitShortHash() const;
    [[nodiscard]] QDateTime commitTime() const;

    [[nodiscard]] const QList<Reference> &references() const;

    [[nodiscard]] Tree tree() const override;
    [[nodiscard]] QString treeTitle() const override;

    [[nodiscard]] git_commit *gitCommit() const;

    [[nodiscard]] Note note() const;
    bool createNote(const QString &message);
    Oid oid() const override;

private:
    QSharedPointer<CommitPrivate> d;

    void clearChildren();
    void setReferences(const QList<Reference> refs);
    void addChild(const QString &childHash);

    friend class LogList;
    friend class Manager;
    friend class CommitsCache;
};
}
