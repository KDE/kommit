/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/types.h>

#include <QSharedPointer>

#include "libkommit_export.h"

namespace Git
{

class Note;
class Tag;
class Oid;
class Tree;
class Commit;

class ObjectPrivate;
class LIBKOMMIT_EXPORT Object
{
public:
    enum class Type {
        Any = GIT_OBJECT_ANY,
        Invalid = GIT_OBJECT_INVALID,
        Commit = GIT_OBJECT_COMMIT,
        Tree = GIT_OBJECT_TREE,
        Blob = GIT_OBJECT_BLOB,
        Tag = GIT_OBJECT_TAG
    };
    Object();
    explicit Object(git_object *obj);
    Object(const Object &other);
    Object &operator=(const Object &other);
    bool operator==(const Object &other) const;
    bool operator!=(const Object &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_object *data() const;
    [[nodiscard]] const git_object *constData() const;

    [[nodiscard]] Type type() const;

    Oid id() const;
    [[nodiscard]] Note toNote() const;
    [[nodiscard]] Tag toTag() const;
    [[nodiscard]] Tree toTree() const;
    [[nodiscard]] Commit toCommit() const;

private:
    QSharedPointer<git_object> d;
};

}
