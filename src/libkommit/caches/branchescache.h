/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <Kommit/AbstractCache>
#include <Kommit/Branch>

#include "libkommit_export.h"
#include "libkommit_global.h"
#include "types.h"

#include <git2/types.h>

#include <QObject>
#include <QScopedPointer>

namespace Git
{

class BranchesCachePrivate;
class Repository;

class LIBKOMMIT_EXPORT BranchesCache : public QObject, public Cache<Branch, git_reference>
{
    Q_OBJECT

public:
    explicit BranchesCache(Repository *manager);
    ~BranchesCache() override;

    [[nodiscard]] ListType allBranches(BranchType type = BranchType::AllBranches);
    [[nodiscard]] DataType findByName(const QString &key);
    [[nodiscard]] QStringList names(BranchType type = BranchType::AllBranches);
    [[nodiscard]] DataType current();
    [[nodiscard]] QString currentName();

    /**
     * The branch this repository is built on, by what the repository itself says rather than
     * by the names such a branch usually has.
     *
     * Cloning writes refs/remotes/<remote>/HEAD and git remote set-head keeps it, so that is
     * the answer where there is one. A repository nobody cloned has init.defaultBranch to say
     * what it would have been given, and failing both, the branch being worked on.
     */
    [[nodiscard]] QString defaultName();

    bool create(const QString &name);
    bool remove(DataType branch);

protected:
    void clearChildData() override;

Q_SIGNALS:
    void currentChanged(DataType oldBranch, DataType newBranch);
    void added(DataType branch);
    void removed(DataType branch);
    void reseted();

private:
    QScopedPointer<BranchesCachePrivate> d_ptr;
    Q_DECLARE_PRIVATE(BranchesCache)
};

}
