/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/types.h>

#include <QString>

#include "entities/tree.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT Index
{
public:
    explicit Index(git_index *index);

    bool addByPath(const QString &path) const;
    bool removeByPath(const QString &path) const;

    bool write();
    QSharedPointer<Tree> tree() const;

private:
    git_index *const ptr;
};

}
