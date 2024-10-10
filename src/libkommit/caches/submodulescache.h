/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "caches/abstractcache.h"
#include "entities/submodule.h"
#include "libkommit_export.h"

namespace Git
{

class AddSubmoduleOptions;

class LIBKOMMIT_EXPORT SubmodulesCache : public QObject, public Cache<Submodule, git_submodule>
{
    Q_OBJECT

public:
    explicit SubmodulesCache(Repository *manager);

    DataType add(const AddSubmoduleOptions &options);

    [[nodiscard]] QList<QSharedPointer<Submodule>> allSubmodules();
    [[nodiscard]] QSharedPointer<Submodule> findByName(const QString &name);
    [[nodiscard]] DataType findByPtr(git_submodule *ptr, bool *isNew = nullptr);

protected:
    void clearChildData() override;

Q_SIGNALS:
    void added(DataType submodule);
};

}
