/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractcache.h"
#include "entities/branch.h"
#include "entities/commit.h"
#include "entities/remote.h"
#include "entities/tag.h"
#include "gitglobal_p.h"

#include <git2/branch.h>
#include <git2/commit.h>
#include <git2/notes.h>
#include <git2/object.h>
#include <git2/remote.h>
#include <git2/revparse.h>
#include <git2/tag.h>

namespace Git
{

template<class T>
AbstractCache<T>::AbstractCache(git_repository *repo)
    : mRepo{repo}
{
}

template<class T>
AbstractCache<T>::~AbstractCache()
{
}

template<class T>
template<typename... Args>
QSharedPointer<T> AbstractCache<T>::findOrCreate(const QString &key, Args... args)
{
    if (mMap.contains(key))
        return mMap.value(key);

    auto en = QSharedPointer<T>{new T{args...}};
    mMap.insert(key, en);
    return en;
}

template<class T>
QSharedPointer<T> AbstractCache<T>::findOrLookup(const QString &key, bool *isNew)
{
    if (mMap.contains(key)) {
        if (isNew)
            *isNew = false;
        return mMap.value(key);
    }

    auto en = QSharedPointer<T>{lookup(key)};
    mMap.insert(key, en);

    if (isNew)
        *isNew = true;
    return en;
}

template<class T>
QList<QSharedPointer<T>> AbstractCache<T>::values() const
{
    return mMap.values();
}

template<class T>
void AbstractCache<T>::insert(const QString &key, QSharedPointer<T> obj)
{
    mMap.insert(key, obj);
}

template<class T>
T *AbstractCache<T>::lookup(const QString &key, git_repository *repo)
{
    Q_UNUSED(key)
    Q_UNUSED(repo)
    return nullptr;
}

CommitsCache::CommitsCache(git_repository *repo)
    : AbstractCache<Commit>{repo}
{
}

Commit *CommitsCache::lookup(const QString &key, git_repository *repo)
{
    git_commit *commit;
    git_object *commitObject;
    BEGIN
    STEP git_revparse_single(&commitObject, repo, key.toLatin1().constData());
    STEP git_commit_lookup(&commit, repo, git_object_id(commitObject));
    if (IS_OK)
        return new Commit{commit};
    return nullptr;
}

BranchesCache::BranchesCache(git_repository *repo)
    : Git::AbstractCache<Branch>{repo}
{
}

Branch *BranchesCache::lookup(const QString &key, git_repository *repo)
{
    git_reference *ref;
    BEGIN
    STEP git_branch_lookup(&ref, repo, key.toLocal8Bit().data(), GIT_BRANCH_ALL);

    if (IS_OK)
        return new Branch{ref};
    return nullptr;
}

TagsCache::TagsCache(git_repository *repo)
    : Git::AbstractCache<Tag>{repo}
{
}

Tag *TagsCache::lookup(const QString &key, git_repository *repo)
{
    git_tag *tag;
    git_object *tagObject;

    BEGIN
    STEP git_revparse_single(&tagObject, repo, key.toLatin1().constData());
    STEP git_tag_lookup(&tag, repo, git_object_id(tagObject));

    if (IS_OK)
        return new Tag{tag};
    return nullptr;
}

NotesCache::NotesCache(git_repository *repo)
    : Git::AbstractCache<Note>{repo}
{
}

RemotesCache::RemotesCache(git_repository *repo)
    : Git::AbstractCache<Remote>{repo}
{
}

Remote *RemotesCache::lookup(const QString &key, git_repository *repo)
{
    git_remote *remote;
    BEGIN
    STEP git_remote_lookup(&remote, repo, key.toLocal8Bit().data());
    if (IS_OK)
        return new Remote{remote};
    return nullptr;
}
};

#include "moc_abstractcache.cpp"
