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

class Repository;

namespace Impl
{
git_repository *getRepo(Repository *manager);
}

template<class ObjectType, class PtrType>
class Cache
{
public:
    using DataType = ObjectType;
    using ListType = QList<ObjectType>;

    explicit Cache(Repository *git);
    virtual ~Cache();

    DataType findByPtr(PtrType *ptr, bool *isNew = nullptr);

    bool insert(PtrType *ptr, const ObjectType &obj);

    int size() const;
    const DataType &at(int index) const;
    void clear();

protected:
    virtual void clearChildData() = 0;
    bool removeFromList(PtrType *ptr);

    Repository *manager;
    ListType mList;
    QHash<PtrType *, DataType> mHash;
};

template<class ObjectType, class PtrType>
class OidCache : public Cache<ObjectType, PtrType>
{
public:
    using GitLookupFunc = int (*)(PtrType **, git_repository *, const git_oid *);

    explicit OidCache(Repository *git);
    OidCache(Repository *git, GitLookupFunc func);

    ObjectType findByOid(const git_oid *oid, bool *isNew = nullptr);

protected:
    GitLookupFunc gitLookupFunc{nullptr};
};

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE OidCache<ObjectType, PtrType>::OidCache(Repository *git)
    : Git::Cache<ObjectType, PtrType>{git}
{
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE OidCache<ObjectType, PtrType>::OidCache(Repository *git, GitLookupFunc func)
    : Git::Cache<ObjectType, PtrType>{git}
    , gitLookupFunc{func}
{
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE ObjectType OidCache<ObjectType, PtrType>::findByOid(const git_oid *oid, bool *isNew)
{
    PtrType *ptr;
    auto r = gitLookupFunc(&ptr, Impl::getRepo(Cache<ObjectType, PtrType>::manager), oid);
    if (!r)
        return Cache<ObjectType, PtrType>::findByPtr(ptr, isNew);

    if (isNew)
        *isNew = false;
    return ObjectType{};
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE Cache<ObjectType, PtrType>::Cache(Repository *git)
    : manager{git}
{
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE Cache<ObjectType, PtrType>::~Cache()
{
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE ObjectType Cache<ObjectType, PtrType>::findByPtr(PtrType *ptr, bool *isNew)
{
    if (mHash.contains(ptr)) {
        if (isNew)
            *isNew = false;
        return mHash.value(ptr);
    }

    ObjectType entity{ptr};
    mList << entity;
    mHash.insert(ptr, entity);

    if (isNew)
        *isNew = true;
    return entity;
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE bool Cache<ObjectType, PtrType>::insert(PtrType *ptr, const ObjectType &obj)
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
Q_OUTOFLINE_TEMPLATE const ObjectType &Cache<ObjectType, PtrType>::at(int index) const
{
    return std::move(mList.at(index));
}

template<class ObjectType, class PtrType>
Q_OUTOFLINE_TEMPLATE void Cache<ObjectType, PtrType>::clear()
{
    mList.clear();
    mHash.clear();
    clearChildData();
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
};
