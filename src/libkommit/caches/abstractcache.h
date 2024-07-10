/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QHash>
#include <QMap>
#include <QObject>
#include <QSharedPointer>

#include <git2/types.h>

#include "libkommit_export.h"

namespace Git
{

class Manager;
class Branch;
class Note;
class Remote;
class Commit;
class Tag;

namespace Impl
{
git_repository *getRepo(Manager *manager);
}

template<class T>
using EntitiesList = QList<QSharedPointer<T>>;

template<class ObjectType>
struct CacheData {
    Manager *manager;
    EntitiesList<ObjectType> list;
};

template<class T>
class OidLookupProvider;

template<class ObjectType, class PtrType>
class LIBKOMMIT_EXPORT Cache
{
public:
    using DataMember = QSharedPointer<ObjectType>;
    using DataList = QList<QSharedPointer<ObjectType>>;
    explicit Cache(Manager *git);
    virtual ~Cache();

    DataMember findByPtr(PtrType *ptr, bool *isNew = nullptr);

    bool insert(PtrType *ptr, QSharedPointer<ObjectType> obj);

    int size() const;
    const DataMember &at(int index) const;
    void clear();

protected:
    bool removeFromList(PtrType *ptr);

    Manager *manager;
    DataList mList;
    QHash<PtrType *, DataMember> mHash;
};

template<class ObjectType, class PtrType>
class LIBKOMMIT_EXPORT OidCache : public Cache<ObjectType, PtrType>
{
public:
    using GitLookupFunc = int (*)(PtrType **, git_repository *, const git_oid *);

    explicit OidCache(Manager *git);
    OidCache(Manager *git, GitLookupFunc func);

    QSharedPointer<ObjectType> findByOid(const git_oid *oid, bool *isNew = nullptr);

protected:
    GitLookupFunc gitLookupFunc{nullptr};
};

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE OidCache<ObjectType, PtrType>::OidCache(Manager *git)
    : Git::Cache<ObjectType, PtrType>{git}
{
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE OidCache<ObjectType, PtrType>::OidCache(Manager *git, GitLookupFunc func)
    : Git::Cache<ObjectType, PtrType>{git}
    , gitLookupFunc{func}
{
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE QSharedPointer<ObjectType> OidCache<ObjectType, PtrType>::findByOid(const git_oid *oid, bool *isNew)
{
    PtrType *ptr;
    auto r = gitLookupFunc(&ptr, Impl::getRepo(Cache<ObjectType, PtrType>::manager), oid);
    if (!r)
        return Cache<ObjectType, PtrType>::findByPtr(ptr, isNew);

    if (isNew)
        *isNew = false;
    return QSharedPointer<ObjectType>{};
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE Cache<ObjectType, PtrType>::Cache(Manager *git)
    : manager{git}
{
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE Cache<ObjectType, PtrType>::~Cache()
{
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE QSharedPointer<ObjectType> Cache<ObjectType, PtrType>::findByPtr(PtrType *ptr, bool *isNew)
{
    if (mHash.contains(ptr)) {
        if (isNew)
            *isNew = false;
        return mHash.value(ptr);
    }

    auto entity = DataMember{new ObjectType{ptr}};
    mList << entity;
    mHash.insert(ptr, entity);

    if (isNew)
        *isNew = true;
    return entity;
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE bool Cache<ObjectType, PtrType>::insert(PtrType *ptr, QSharedPointer<ObjectType> obj)
{
    if (mHash.contains(ptr))
        return false;

    mHash.insert(ptr, obj);
    mList << obj;

    return true;
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE int Cache<ObjectType, PtrType>::size() const
{
    return mList.size();
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE const QSharedPointer<ObjectType> &Cache<ObjectType, PtrType>::at(int index) const
{
    return std::move(mList.at(index));
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE void Cache<ObjectType, PtrType>::clear()
{
    mList.clear();
    mHash.clear();
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE bool Cache<ObjectType, PtrType>::removeFromList(PtrType *ptr)
{
    if (!mHash.contains(ptr))
        return false;

    mList.removeOne(mHash.value(ptr));
    mHash.remove(ptr);
    return true;
}

using BranchCache = Cache<Branch, git_reference>;
using TagCache = Cache<Tag, git_tag>;
using NoteCache = Cache<Note, git_note>;
using RemoteCache = Cache<Remote, git_remote>;
using CommitCache = Cache<Commit, git_commit>;

template<class T>
class LIBKOMMIT_EXPORT AbstractCache
{
public:
    explicit AbstractCache(git_repository *repo);
    explicit AbstractCache(Manager *parent);
    virtual ~AbstractCache();

    void clear();
    QSharedPointer<T> at(int index) const;

    Q_REQUIRED_RESULT int count() const;

    // template<typename... Args>
    // QSharedPointer<T> findOrCreate(const QString &key, Args... args) const
    // {
    //     if (mMap.contains(key))
    //         return mMap.value(key);

    //     auto en = QSharedPointer<T>{new T{args...}};
    //     mMap.insert(key, en);
    //     return en;
    // }
    QList<QSharedPointer<T>> values() const
    {
        return mList;
    }

    Q_REQUIRED_RESULT git_repository *repo() const;
    void setRepo(git_repository *newRepo);

protected:
    virtual void inserted(QSharedPointer<T>)
    {
    }
    git_repository *mRepo;
    QList<QSharedPointer<T>> mList;
};

template<class T>
class OidLookupProvider
{
public:
    QSharedPointer<T> findOrLookupByOid(git_oid *oid, bool *isNew = nullptr)
    {
        if (mOidHash.contains(oid)) {
            if (isNew)
                *isNew = false;
            return mOidHash.value(oid);
        }

        auto en = QSharedPointer<T>{lookup(oid, nullptr)};
        mOidHash.insert(oid, en);
        static_cast<AbstractCache<T>>(this).inserted(en);

        if (isNew)
            *isNew = true;
        return en;
    }

    virtual ~OidLookupProvider()
    {
    }

protected:
    virtual T *lookup(git_oid *oid, git_repository *repo) = 0;
    QHash<git_oid *, QSharedPointer<T>> mOidHash;
};

template<class T>
class StringLookupProvider
{
public:
    QSharedPointer<T> findOrLookupByKey(const QString &key, bool *isNew = nullptr)
    {
        if (mMap.contains(key)) {
            if (isNew)
                *isNew = false;
            return mMap.value(key);
        }

        auto en = QSharedPointer<T>{lookup(key, nullptr)};
        mMap.insert(key, en);
        // static_cast<AbstractCache<T>>(this)->mList << en;
        if (isNew)
            *isNew = true;
        return en;
    }
    void insert(const QString &key, QSharedPointer<T> obj)
    {
        mMap.insert(key, obj);
    }

    virtual ~StringLookupProvider()
    {
    }

protected:
    virtual T *lookup(const QString &key, git_repository *repo) = 0;
    QMap<QString, QSharedPointer<T>> mMap;
};

template<class T, class Ptr>
class LookupProvider
{
    QSharedPointer<T> find(Ptr *oid, bool *isNew = nullptr)
    {
        if (mHash.contains(oid)) {
            if (isNew)
                *isNew = false;
            return mHash.value(oid);
        }

        auto en = QSharedPointer<T>{lookup(oid, nullptr)};
        mHash.insert(oid, en);
        static_cast<AbstractCache<T>>(this).inserted(en);

        if (isNew)
            *isNew = true;
        return en;
    }
    virtual T *lookup(Ptr *oid, git_repository *repo) = 0;

    QHash<Ptr *, QSharedPointer<T>> mHash;
};

template<class T>
Q_OUTOFLINE_TEMPLATE AbstractCache<T>::AbstractCache(git_repository *repo)
    : mRepo{repo}
{
}

template<class T>
inline AbstractCache<T>::AbstractCache(Manager *parent)
{
}

template<class T>
Q_OUTOFLINE_TEMPLATE AbstractCache<T>::~AbstractCache()
{
}

template<class T>
Q_OUTOFLINE_TEMPLATE void AbstractCache<T>::clear()
{
    mList.clear();
    // mMap.clear();
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

};
