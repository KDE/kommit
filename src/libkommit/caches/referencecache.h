/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <Kommit/AbstractCache>
#include <Kommit/Note>
#include <Kommit/Reference>

#include "libkommit_export.h"

#include <git2/types.h>

#include <QMap>
#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>

namespace Git
{

class Commit;
class Remote;
class ReferenceCachePrivate;
class LIBKOMMIT_EXPORT ReferenceCache : public Cache<Reference, git_reference>
{
public:
    explicit ReferenceCache(Repository *parent);
    ~ReferenceCache() override;

    DataType findByName(const QString &name);
    DataType findForNote(const Note &note);
    DataType findForBranch(const Branch &branch);
    DataType findForTag(const Tag &tag);
    DataType findForRemote(const Remote &remote);
    ListType findForCommit(const Commit &commit);

    void forEach(std::function<void(DataType)> callback) const;

protected:
    void clearChildData() override;

private:
    void fill();

    QScopedPointer<ReferenceCachePrivate> d_ptr;
    Q_DECLARE_PRIVATE(ReferenceCache)
};
class ReferenceCachePrivate
{
    ReferenceCache *q_ptr;
    Q_DECLARE_PUBLIC(ReferenceCache)

public:
    explicit ReferenceCachePrivate(ReferenceCache *parent);

    QList<Reference> list;
    QMultiMap<Commit, Reference> dataByCommit;
    void fill();
};
}
