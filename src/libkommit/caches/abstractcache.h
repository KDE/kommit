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

class Manager;

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

    template<typename... Args>
    QSharedPointer<T> findOrCreate(const QString &key, Args... args) const
    {
        if (mMap.contains(key))
            return mMap.value(key);

        auto en = QSharedPointer<T>{new T{args...}};
        mMap.insert(key, en);
        return en;
    }
    QSharedPointer<T> findOrLookup(const QString &key, bool *isNew = nullptr)
    {
        if (mMap.contains(key)) {
            if (isNew)
                *isNew = false;
            return mMap.value(key);
        }

        auto en = QSharedPointer<T>{lookup(key, mRepo)};
        mMap.insert(key, en);

        if (isNew)
            *isNew = true;
        return en;
    }

    QList<QSharedPointer<T>> values() const
    {
        return mMap.values();
    }
    void insert(const QString &key, QSharedPointer<T> obj)
    {
        mMap.insert(key, obj);
    }

    Q_REQUIRED_RESULT git_repository *repo() const;
    void setRepo(git_repository *newRepo);

protected:
    virtual T *lookup(const QString &key, git_repository *repo)
    {
        Q_UNUSED(key)
        Q_UNUSED(repo)
        return nullptr;
    }
    git_repository *mRepo;

private:
    QMap<QString, QSharedPointer<T>> mMap;
    QList<QSharedPointer<T>> mList;
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

};
