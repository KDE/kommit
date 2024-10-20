/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/oid.h>

#include <QString>

#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT Oid
{
public:
    Oid();
    explicit Oid(const git_oid *oid);
    explicit Oid(const git_oid oid);
    ~Oid();

    [[nodiscard]] QString toString() const;
    [[nodiscard]] bool isNull() const;

    void copyTo(git_oid *oid);

    [[nodiscard]] const git_oid *oidPtr() const;

private:
    git_oid mOid;
};

}

bool operator==(const Git::Oid &oid, const QString &hash);
bool operator!=(const Git::Oid &oid, const QString &hash);
