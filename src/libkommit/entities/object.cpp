/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "object.h"

#include "note.h"
#include "oid.h"
#include "tag.h"
#include "tree.h"

#include <git2/branch.h>
#include <git2/commit.h>
#include <git2/notes.h>
#include <git2/object.h>
#include <git2/tag.h>
#include <git2/tree.h>

namespace Git
{

Object::Object()
    : d{}
{
}

Object::Object(git_object *obj)
    : d{obj, git_object_free}
{
}

Object::Object(const Object &other)
    : d{other.d}
{
}

Object &Object::operator=(const Object &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Object::operator==(const Object &other) const
{
    return d.data() == other.d.data();
}

bool Object::operator!=(const Object &other) const
{
    return !(*this == other);
}

bool Object::isNull() const
{
    return d.isNull();
}

git_object *Object::data() const
{
    return d.data();
}

const git_object *Object::constData() const
{
    return d.data();
}

Object::Type Object::type() const
{
    return static_cast<Type>(git_object_type(d.data()));
}

Oid Object::id() const
{
    return Oid{git_object_id(d.data())};
}

Note Object::toNote() const
{
    auto oid = git_object_id(d.data());
    auto repo = git_object_owner(d.data());
    git_note *note;
    git_note_read(&note, repo, NULL, oid);
    return Note{note};
}

Tag Object::toTag() const
{
    auto oid = git_object_id(d.data());
    auto repo = git_object_owner(d.data());
    git_tag *tag;
    git_tag_lookup(&tag, repo, oid);
    return Tag{tag};
}

Tree Object::toTree() const
{
    auto oid = git_object_id(d.data());
    auto repo = git_object_owner(d.data());
    git_tree *tree;
    git_tree_lookup(&tree, repo, oid);
    return Tree{tree};
}

Commit Object::toCommit() const
{
    auto oid = git_object_id(d.data());
    auto repo = git_object_owner(d.data());
    git_commit *commit;
    git_commit_lookup(&commit, repo, oid);
    return Commit{commit};
}

} // namespace Git
