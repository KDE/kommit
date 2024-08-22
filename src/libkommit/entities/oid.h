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
    explicit Oid(const git_oid *oid);

    [[nodiscard]] QString toString() const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] const git_oid *oidPtr() const;

private:
    const git_oid *mOidPtr;
};

};

bool operator==(const Git::Oid &oid, const QString &hash);
bool operator!=(const Git::Oid &oid, const QString &hash);
