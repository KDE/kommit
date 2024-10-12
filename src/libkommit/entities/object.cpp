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

Object::Object(git_object *obj)
    : mGitObjectPtr{obj}
{
}

Object::Type Object::type() const
{
    return static_cast<Type>(git_object_type(mGitObjectPtr));
}

QSharedPointer<Oid> Object::id() const
{
    return QSharedPointer<Oid>{new Oid{git_object_id(mGitObjectPtr)}};
}

QSharedPointer<Note> Object::toNote() const
{
    auto oid = git_object_id(mGitObjectPtr);
    auto repo = git_object_owner(mGitObjectPtr);
    git_note *note;
    git_note_read(&note, repo, NULL, oid);
    return QSharedPointer<Note>{new Note{note}};
}

QSharedPointer<Tag> Object::toTag() const
{
    auto oid = git_object_id(mGitObjectPtr);
    auto repo = git_object_owner(mGitObjectPtr);
    git_tag *tag;
    git_tag_lookup(&tag, repo, oid);
    return QSharedPointer<Tag>{new Tag{tag}};
}

QSharedPointer<Tree> Object::toTree() const
{
    auto oid = git_object_id(mGitObjectPtr);
    auto repo = git_object_owner(mGitObjectPtr);
    git_tree *tree;
    git_tree_lookup(&tree, repo, oid);
    return QSharedPointer<Tree>{new Tree{tree}};
}

QSharedPointer<Commit> Object::toCommit() const
{
    auto oid = git_object_id(mGitObjectPtr);
    auto repo = git_object_owner(mGitObjectPtr);
    git_commit *commit;
    git_commit_lookup(&commit, repo, oid);
    return QSharedPointer<Commit>{new Commit{commit}};
}

git_object *Object::objectPtr() const
{
    return mGitObjectPtr;
}
}
