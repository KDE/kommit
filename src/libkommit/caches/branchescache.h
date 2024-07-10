/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "caches/abstractcache.h"
#include "libkommit_export.h"

#include <QObject>
#include <git2/types.h>

namespace Git
{
class Branch;
class BranchesCachePrivate;
class Manager;
class Reference;

class LIBKOMMIT_EXPORT BranchesCache : public QObject, public BranchCache
{
    Q_OBJECT

public:
    using DataMember = QSharedPointer<Branch>;
    using DataList = QList<DataMember>;

    enum class BranchType {
        LocalBranch,
        RemoteBranch,
        AllBranches,
    };

    explicit BranchesCache(Manager *manager);
    virtual ~BranchesCache();

    DataMember find(const QString &key);
    DataMember find(git_reference *ref);
    DataMember find(Reference *ref);
    bool create(const QString &name);
    bool remove(DataMember branch);

    DataList allBranches(BranchType type = BranchType::AllBranches);
    QStringList names(BranchType type = BranchType::AllBranches);
    DataMember current();

    void clear();

protected:
    Branch *lookup(const QString &key, git_repository *repo);
    Branch *lookup(git_oid *oid, git_repository *repo);
    void inserted(QSharedPointer<Branch> branch);

Q_SIGNALS:
    void currentChanged(DataMember oldBranch, DataMember newBranch);
    void added(DataMember branch);
    void removed(DataMember branch);
    void reseted();

private:
    BranchesCachePrivate *d_ptr;
    Q_DECLARE_PRIVATE(BranchesCache)
};
};
