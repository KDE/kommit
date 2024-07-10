/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchescache.h"
#include "entities/branch.h"
#include "entities/reference.h"
#include "gitglobal_p.h"
#include "gitmanager.h"

#include <git2/branch.h>

#include <QDebug>
#include <QHash>

namespace Git
{

class BranchesCachePrivate
{
public:
    BranchesCache *q_ptr;
    Q_DECLARE_PUBLIC(BranchesCache)

    Manager *manager;
    QList<BranchesCache::DataMember> list;
    QMap<QString, BranchesCache::DataMember> dataByName;
    QHash<git_reference *, BranchesCache::DataMember> dataByRef;

    BranchesCachePrivate(BranchesCache *parent, Manager *manager);
    void add(BranchesCache::DataMember newItem);
    bool remove(BranchesCache::DataMember branch);
};

BranchesCache::BranchesCache(Manager *manager)
    : QObject{manager}
    , Git::BranchCache{manager}
    , d_ptr{new BranchesCachePrivate{this, manager}}
{
}

BranchesCache::~BranchesCache()
{
    Q_D(BranchesCache);
    delete d;
}

BranchesCache::DataMember BranchesCache::find(const QString &key)
{
    Q_D(BranchesCache);

    if (d->dataByName.contains(key))
        return d->dataByName.value(key);

    git_reference *ref;
    BEGIN
    STEP git_branch_lookup(&ref, d->manager->repoPtr(), key.toLocal8Bit().constData(), GIT_BRANCH_ALL);
    if (IS_OK) {
        auto b = DataMember{new Branch{ref}};
        d->add(b);
        return b;
    }
    return {};
}

BranchesCache::DataMember BranchesCache::find(git_reference *ref)
{
    Q_D(BranchesCache);
    if (d->dataByRef.contains(ref))
        return d->dataByRef.value(ref);
    auto b = DataMember{new Branch{ref}};
    d->add(b);

    Q_EMIT added(b);
    return b;
}

BranchesCache::DataMember BranchesCache::find(Reference *ref)
{
    return find(ref->refPtr());
}

bool BranchesCache::create(const QString &name)
{
    return false;
}

bool BranchesCache::remove(DataMember branch)
{
    Q_D(BranchesCache);

    if (d->remove(branch))
        Q_EMIT removed(branch);
    return false;
}

QList<BranchesCache::DataMember> BranchesCache::allBranches(BranchesCache::BranchType type)
{
    Q_D(BranchesCache);

    git_branch_t t{GIT_BRANCH_ALL};
    git_branch_iterator *it;
    switch (type) {
    case BranchType::AllBranches:
        t = GIT_BRANCH_ALL;
        break;
    case BranchType::LocalBranch:
        t = GIT_BRANCH_LOCAL;
        break;
    case BranchType::RemoteBranch:
        t = GIT_BRANCH_REMOTE;
        break;
    }
    git_branch_iterator_new(&it, d->manager->repoPtr(), t);

    QList<BranchesCache::DataMember> list;
    if (!it) {
        return list;
    }
    git_reference *ref;
    git_branch_t b;

    while (!git_branch_next(&ref, &b, it))
        list << find(ref);

    git_branch_iterator_free(it);

    return list;
}

QStringList BranchesCache::names(BranchType type)
{
    QStringList list;
    auto branches = allBranches(type);
    for (auto const &branch : branches)
        list << branch->name();
    return list;
}

BranchesCache::DataMember BranchesCache::current()
{
    Q_D(BranchesCache);

    git_reference *ref;
    BEGIN
    STEP git_repository_head(&ref, d->manager->repoPtr());
    if (IS_ERROR) {
        PRINT_ERROR;
        return {};
    }

    auto b = find(ref);
    return b;
}

void BranchesCache::clear()
{
    Q_D(BranchesCache);
    d->dataByName.clear();
    d->dataByRef.clear();
    d->list.clear();
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

Branch *BranchesCache::lookup(git_oid *oid, git_repository *repo)
{
    git_reference *branch;
    auto name = git_oid_tostr_s(oid);
    if (git_branch_lookup(&branch, repo, name, GIT_BRANCH_ALL))
        return nullptr;
    return new Branch{branch};
}

void BranchesCache::inserted(QSharedPointer<Branch> branch)
{
    // AbstractCache<Branch>::mList << branch;
    // StringLookupProvider<Branch>::mMap.insert(branch->name(), branch);
    //  OidLookupProvider<Branch>{}
}

BranchesCachePrivate::BranchesCachePrivate(BranchesCache *parent, Manager *manager)
    : q_ptr{parent}
    , manager{manager}
{
}

void BranchesCachePrivate::add(BranchesCache::DataMember newItem)
{
    if (dataByName.contains(newItem->name()))
        return;

    list.append(newItem);
    dataByName.insert(newItem->name(), newItem);
    dataByRef.insert(newItem->refPtr(), newItem);
}

bool BranchesCachePrivate::remove(BranchesCache::DataMember branch)
{
    if (!list.contains(branch))
        return false;

    list.removeOne(branch);
    dataByName.remove(branch->name());
    dataByRef.remove(branch->refPtr());
    return true;
}

}

#include "moc_branchescache.cpp"
