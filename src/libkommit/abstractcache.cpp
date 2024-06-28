#include "abstractcache.h"

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
    if (mData.contains(key))
        return mData.value(key);

    auto en = QSharedPointer<T>{new T{args...}};
    mData.insert(key, en);
    return en;
}

template<class T>
QSharedPointer<T> AbstractCache<T>::findOrCreate(const QString &key)
{
    if (mData.contains(key))
        return mData.value(key);

    auto en = QSharedPointer<T>{create(key)};
    mData.insert(key, en);
    return en;
}

template<class T>
T *AbstractCache<T>::create(const QString &key)
{
    Q_UNUSED(key)
    return nullptr;
}

template<class T>
void AbstractCache<T>::clear()
{
    mData.clear();
}

template<class T>
int AbstractCache<T>::count() const
{
    return mData.size();
}

CommitsCache::CommitsCache(git_repository *repo)
    : AbstractCache<Commit>{repo}
{
}

Commit *CommitsCache::create(const QString &key)
{
    Q_UNUSED(key)
    return nullptr;
}

};
