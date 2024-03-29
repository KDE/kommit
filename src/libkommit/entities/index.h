/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/types.h>

#include <QString>

#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT Index
{
public:
    Index(git_index *index);

    Q_REQUIRED_RESULT bool addByPath(const QString &path) const;
    Q_REQUIRED_RESULT bool removeByPath(const QString &path) const;

private:
    git_index *ptr;
};

}
