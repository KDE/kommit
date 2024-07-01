/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <git2/types.h>

#include "libkommit_export.h"

namespace Git
{

class Commit;
class Branch;
class Tag;
class Note;
class Remote;

template<class T>
class LIBKOMMIT_EXPORT AbstractCache
{
public:
    explicit AbstractCache(git_repository *repo);
    virtual ~AbstractCache();

    void clear();
    QSharedPointer<T> at(int index) const;

    Q_REQUIRED_RESULT int count() const;

    template<typename... Args>
    QSharedPointer<T> findOrCreate(const QString &key, Args... args);
    QSharedPointer<T> findOrLookup(const QString &key, bool *isNew = nullptr);

    QList<QSharedPointer<T>> values() const;
    void insert(const QString &key, QSharedPointer<T> obj);

    Q_REQUIRED_RESULT git_repository *repo() const;
    void setRepo(git_repository *newRepo);

protected:
    virtual T *lookup(const QString &key, git_repository *repo);
    git_repository *mRepo;

private:
    QMap<QString, QSharedPointer<T>> mMap;
    QList<QSharedPointer<T>> mList;
};

template<class T>
Q_OUTOFLINE_TEMPLATE void AbstractCache<T>::clear()
{
    mList.clear();
    mMap.clear();
}

template<class T>
Q_OUTOFLINE_TEMPLATE QSharedPointer<T> AbstractCache<T>::at(int index) const
{
    return mList.at(index);
}

template<class T>
Q_OUTOFLINE_TEMPLATE int AbstractCache<T>::count() const
{
    return mList.size();
}

template<class T>
Q_OUTOFLINE_TEMPLATE git_repository *AbstractCache<T>::repo() const
{
    return mRepo;
}

template<class T>
Q_OUTOFLINE_TEMPLATE void AbstractCache<T>::setRepo(git_repository *newRepo)
{
    mRepo = newRepo;
}

class LIBKOMMIT_EXPORT CommitsCache : public QObject, public AbstractCache<Commit>
{
    Q_OBJECT
public:
    explicit CommitsCache(git_repository *repo);

protected:
    Commit *lookup(const QString &key, git_repository *repo) override;
};

class LIBKOMMIT_EXPORT BranchesCache : public AbstractCache<Branch>
{
public:
    explicit BranchesCache(git_repository *repo);

protected:
    Branch *lookup(const QString &key, git_repository *repo) override;
};

class LIBKOMMIT_EXPORT TagsCache : public AbstractCache<Tag>
{
public:
    explicit TagsCache(git_repository *repo);

protected:
    Tag *lookup(const QString &key, git_repository *repo) override;
};

class LIBKOMMIT_EXPORT RemotesCache : public AbstractCache<Remote>
{
public:
    explicit RemotesCache(git_repository *repo);

protected:
    Remote *lookup(const QString &key, git_repository *repo) override;
};

class LIBKOMMIT_EXPORT NotesCache : public AbstractCache<Note>
{
public:
    explicit NotesCache(git_repository *repo);
};
}
