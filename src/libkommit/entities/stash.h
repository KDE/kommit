/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "interfaces.h"
#include "libkommit_export.h"

#include <QDateTime>
#include <QSharedPointer>
#include <QString>

#include <git2/types.h>

namespace Git
{

class Signature;
class Manager;
class Commit;
class StashPrivate;
class LIBKOMMIT_EXPORT Stash : IOid
{
public:
    Stash(Manager *manager, size_t index, const char *message, const git_oid *stash_id);
    ~Stash();

    [[nodiscard]] const QString &message() const;
    [[nodiscard]] QSharedPointer<Commit> commit();
    [[nodiscard]] size_t index() const;
    [[nodiscard]] QSharedPointer<Oid> oid() const override;

private:
    StashPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(Stash)
};

} // namespace Git
