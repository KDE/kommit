/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <git2/types.h>

#include "abstractcache.h"
#include "entities/remote.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT RemotesCache : public QObject, public Cache<Remote, git_remote>
{
    Q_OBJECT

public:
    explicit RemotesCache(Repository *manager);

    ListType allRemotes();
    QStringList allNames();
    DataType findByName(const QString &name);
    [[nodiscard]] bool create(const QString &name, const QString &url);
    [[nodiscard]] bool setUrl(const Remote &remote, const QString &url);

    [[nodiscard]] bool remove(const Remote &remote);
    [[nodiscard]] bool remove(const QString &name);

    [[nodiscard]] bool rename(const Remote &remote, const QString &newName);

protected:
    void clearChildData() override;

Q_SIGNALS:
    void added(Git::Remote commit);
    void removed(Git::Remote commit);
};

}
