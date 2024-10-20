/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchescache.h"
#include "entities/reference.h"
#include "gitglobal_p.h"
#include "repository.h"

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

    Repository *manager;
    BranchesCache::ListType list;
    QMap<QString, BranchesCache::DataType> dataByName;
    QHash<git_reference *, BranchesCache::DataType> dataByRef;

    BranchesCachePrivate(BranchesCache *parent, Repository *manager);
    void add(BranchesCache::DataType newItem);
    bool remove(BranchesCache::DataType branch);
};

BranchesCache::BranchesCache(Repository *manager)
    : QObject{manager}
    , Cache<Branch, git_reference>{manager}
    , d_ptr{new BranchesCachePrivate{this, manager}}
{
}

BranchesCache::~BranchesCache()
{
}

BranchesCache::DataType BranchesCache::findByName(const QString &key)
{
    Q_D(BranchesCache);

    if (d->dataByName.contains(key))
        return d->dataByName.value(key);

    git_reference *ref;
    BEGIN
    STEP git_branch_lookup(&ref, d->manager->repoPtr(), key.toLocal8Bit().constData(), GIT_BRANCH_ALL);
    if (IS_OK) {
        Branch b{ref};
        d->add(b);
        return b;
    }
    return {};
}

bool BranchesCache::create(const QString &name)
{
    git_reference *ref{nullptr};
    git_commit *commit{nullptr};
    git_reference *head{nullptr};

    BEGIN
    STEP git_repository_head(&head, manager->repoPtr());

    if (!head) {
        PRINT_ERROR;
        return false;
    }

    auto targetId = git_reference_target(head);
    STEP git_commit_lookup(&commit, manager->repoPtr(), targetId);
    STEP git_branch_create(&ref, manager->repoPtr(), name.toLocal8Bit().constData(), commit, 0);

    git_reference_free(ref);
    git_reference_free(head);
    git_commit_free(commit);

    PRINT_ERROR;

    return IS_OK;
}

bool BranchesCache::remove(DataType branch)
{
    Q_D(BranchesCache);

    BEGIN
    STEP git_branch_delete(branch.refPtr());

    if (IS_OK && d->remove(branch)) {
        Q_EMIT removed(branch);
        return true;
    }
    return false;
}

BranchesCache::ListType BranchesCache::allBranches(BranchType type)
{
    Q_D(BranchesCache);

    if (!manager->isValid())
        return QList<BranchesCache::DataType>{};

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

    QList<BranchesCache::DataType> list;
    if (!it) {
        return list;
    }
    git_reference *ref;
    git_branch_t b;

    while (!git_branch_next(&ref, &b, it))
        list << findByPtr(ref);

    git_branch_iterator_free(it);

    return list;
}

QStringList BranchesCache::names(BranchType type)
{
    QStringList list;
    auto branches = allBranches(type);
    for (auto const &branch : branches)
        list << branch.name();
    return list;
}

BranchesCache::DataType BranchesCache::current()
{
    Q_D(BranchesCache);

    git_reference *ref;
    BEGIN
    STEP git_repository_head(&ref, d->manager->repoPtr());
    if (IS_ERROR) {
        PRINT_ERROR;
        return Branch{};
    }

    auto b = findByPtr(ref);
    return b;
}

QString BranchesCache::currentName()
{
    git_reference *ref;
    BEGIN
    STEP git_repository_head(&ref, manager->repoPtr());
    if (IS_ERROR) {
        PRINT_ERROR;
        return {};
    }

    QString branchName{git_reference_shorthand(ref)};

    git_reference_free(ref);

    return branchName;
}

void BranchesCache::clearChildData()
{
    Q_D(BranchesCache);
    d->dataByName.clear();
    d->dataByRef.clear();
    d->list.clear();

    Q_EMIT reseted();
}

BranchesCachePrivate::BranchesCachePrivate(BranchesCache *parent, Repository *manager)
    : q_ptr{parent}
    , manager{manager}
{
}

void BranchesCachePrivate::add(BranchesCache::DataType newItem)
{
    if (dataByName.contains(newItem.name()))
        return;

    list.append(newItem);
    dataByName.insert(newItem.name(), newItem);
    dataByRef.insert(newItem.refPtr(), newItem);
}

bool BranchesCachePrivate::remove(BranchesCache::DataType branch)
{
    if (!list.contains(branch))
        return false;

    list.removeOne(branch);
    dataByName.remove(branch.name());
    dataByRef.remove(branch.refPtr());
    return true;
}

}

#include "moc_branchescache.cpp"
