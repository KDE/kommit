/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "caches/abstractcache.h"
#include "entities/branch.h"
#include "libkommit_export.h"
#include "types.h"

#include <QObject>
#include <git2/types.h>

namespace Git
{

class BranchesCachePrivate;
class Manager;
class Reference;

class LIBKOMMIT_EXPORT BranchesCache : public QObject, public Cache<Branch, git_reference>
{
    Q_OBJECT

public:
    using DataMember = QSharedPointer<Branch>;
    using DataList = QList<DataMember>;

    explicit BranchesCache(Manager *manager);
    ~BranchesCache() override;

    [[nodiscard]] DataList allBranches(BranchType type = BranchType::AllBranches);
    [[nodiscard]] DataMember findByName(const QString &key);
    [[nodiscard]] QStringList names(BranchType type = BranchType::AllBranches);
    [[nodiscard]] DataMember current();
    [[nodiscard]] QString currentName();

    bool create(const QString &name);
    bool remove(DataMember branch);

protected:
    void clearChildData() override;

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
