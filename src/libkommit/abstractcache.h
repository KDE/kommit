#pragma once

#include <QMap>
#include <QSharedPointer>
#include <git2/types.h>

#include "libkommit_export.h"

namespace Git
{

class Commit;

template<class T>
class LIBKOMMIT_EXPORT AbstractCache
{
public:
    explicit AbstractCache(git_repository *repo);
    virtual ~AbstractCache();

    void clear();

    [[nodiscard]] int count() const;

    template<typename... Args>
    QSharedPointer<T> findOrCreate(const QString &key, Args... args);
    QSharedPointer<T> findOrCreate(const QString &key);

protected:
    virtual T *create(const QString &key);
    git_repository *mRepo;

private:
    QMap<QString, QSharedPointer<T>> mData;
};

class LIBKOMMIT_EXPORT CommitsCache : public AbstractCache<Commit>
{
public:
    explicit CommitsCache(git_repository *repo);

protected:
    Commit *create(const QString &key) override;
};

};
