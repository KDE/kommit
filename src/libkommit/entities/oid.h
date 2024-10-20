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
    constexpr Oid() noexcept;
    explicit Oid(const git_oid *oid);
    explicit Oid(const git_oid oid);
    Oid(const Oid &other);

    Oid &operator=(const Oid &other);
    bool operator==(const Oid &other);
    bool operator!=(const Oid &other);

    [[nodiscard]] QString toString() const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_oid *data() const;
    [[nodiscard]] const git_oid *constData() const;

private:
    QByteArray d;
};

}

bool operator==(const Git::Oid &oid, const QString &hash);
bool operator!=(const Git::Oid &oid, const QString &hash);
