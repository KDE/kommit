/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/types.h>

#include <QSharedPointer>

namespace Git
{

class Note;
class Tag;
class Oid;
class Tree;
class Commit;

class Object
{
public:
    enum class Type {
        Any = GIT_OBJECT_ANY,
        Invalid = GIT_OBJECT_INVALID,
        Commit = GIT_OBJECT_COMMIT,
        Tree = GIT_OBJECT_TREE,
        Blob = GIT_OBJECT_BLOB,
        Tag = GIT_OBJECT_TAG,
        OfsDelta = GIT_OBJECT_OFS_DELTA,
        RefDelta = GIT_OBJECT_REF_DELTA,
    };

    explicit Object(git_object *obj);

    [[nodiscard]] Type type() const;

    QSharedPointer<Oid> id() const;
    [[nodiscard]] QSharedPointer<Note> toNote() const;
    [[nodiscard]] QSharedPointer<Tag> toTag() const;
    [[nodiscard]] QSharedPointer<Tree> toTree() const;
    [[nodiscard]] QSharedPointer<Commit> toCommit() const;
    [[nodiscard]] git_object *const objectPtr() const;

private:
    git_object *const mGitObjectPtr;
};

}
