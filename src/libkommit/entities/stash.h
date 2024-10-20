/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QDateTime>
#include <QSharedPointer>
#include <QString>

#include <git2/types.h>

#include <Kommit/IOid>

namespace Git
{

class Repository;
class Commit;
class StashPrivate;
class LIBKOMMIT_EXPORT Stash : IOid
{
public:
    Stash();
    Stash(Repository *manager, size_t index, const char *message, const git_oid *stash_id);
    Stash(const Stash &other);
    Stash &operator=(const Stash &other);
    bool operator==(const Stash &other) const;
    bool operator!=(const Stash &other) const;

    [[nodiscard]] const QString &message() const;
    [[nodiscard]] Commit &commit();
    [[nodiscard]] size_t index() const;
    [[nodiscard]] Oid oid() const override;

    [[nodiscard]] bool isNull() const;

private:
    QSharedPointer<StashPrivate> d;
};

} // namespace Git

Q_DECLARE_METATYPE(Git::Stash)
