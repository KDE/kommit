/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcache.h"
#include "libkommit_export.h"

namespace Git
{
class Branch;

class LIBKOMMIT_EXPORT BranchesCache : public AbstractCache<Branch>
{
public:
    explicit BranchesCache(git_repository *repo);

protected:
    Branch *lookup(const QString &key, git_repository *repo) override;
};

};
