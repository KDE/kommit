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

    Q_REQUIRED_RESULT const QString &message() const;
    Q_REQUIRED_RESULT QSharedPointer<Commit> commit();
    Q_REQUIRED_RESULT size_t index() const;
    Q_REQUIRED_RESULT QSharedPointer<Oid> oid() const override;

private:
    StashPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Stash)
};

} // namespace Git
