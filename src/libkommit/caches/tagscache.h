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
#include "entities/tag.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT TagsCache : public OidCache<Tag, git_tag>
{
public:
    explicit TagsCache(Repository *parent);

    [[nodiscard]] Tag find(const QString &key);
    [[nodiscard]] QList<Tag> allTags();
    [[nodiscard]] QStringList allNames();

    void forEach(std::function<void(const Tag &)> cb);
    bool create(const QString &name, const QString &message, const Object &target = Object{}, const Signature &signature = Signature{});
    bool remove(const Tag &tag);

protected:
    void clearChildData() override;

private:
    QHash<git_commit *, Tag> mTagsByCommit;
};

}
