/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/index.h"

#include "types.h"

#include <git2/index.h>
#include <git2/tree.h>

namespace Git
{

Index::Index(git_index *index)
    : ptr{index}
{
}

bool Index::addByPath(const QString &path) const
{
    if (path.startsWith(QLatin1Char('/')))
        return !git_index_add_bypath(ptr, toConstChars(path.mid(1)));
    else
        return !git_index_add_bypath(ptr, toConstChars(path));
}

bool Index::removeByPath(const QString &path) const
{
    return !git_index_remove_bypath(ptr, toConstChars(path));
}

bool Index::removeAll() const
{
    git_strarray arr;
    addToArray(&arr, QStringLiteral("/"));
    return !git_index_remove_all(ptr, &arr, NULL, NULL);
}

bool Index::write()
{
    return !git_index_write(ptr);
}

QSharedPointer<Tree> Index::tree() const
{
    git_tree *tree;
    git_oid oid;

    auto repo = git_index_owner(ptr);
    if (git_tree_lookup(&tree, repo, &oid))
        return nullptr;

    return QSharedPointer<Tree>{new Tree{tree}};
}
}
