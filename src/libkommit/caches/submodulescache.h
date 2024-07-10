/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "caches/abstractcache.h"
#include "libkommit_export.h"

namespace Git
{
class Submodule;
class AddSubmoduleOptions;

class LIBKOMMIT_EXPORT SubmodulesCache : public QObject, public Cache<Submodule, git_submodule>
{
    Q_OBJECT

public:
    explicit SubmodulesCache(Manager *manager);

    DataMember create(const AddSubmoduleOptions &options);
    QList<QSharedPointer<Submodule>> allSubmodules();
    QSharedPointer<Submodule> findByName(const QString &name);

Q_SIGNALS:
    void added(DataMember submodule);
};

}
