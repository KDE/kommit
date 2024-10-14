/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcache.h"
#include "entities/reference.h"
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
    ~ReferenceCache();

    DataType findByName(const QString &name);
    DataType findForNote(QSharedPointer<Note> note);
    DataType findForBranch(QSharedPointer<Branch> branch);
    DataType findForTag(QSharedPointer<Tag> tag);
    DataType findForRemote(QSharedPointer<Remote> remote);
    ListType findForCommit(QSharedPointer<Commit> commit);

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
    ReferenceCachePrivate(ReferenceCache *parent);

    QList<QSharedPointer<Reference>> list;
    QMultiMap<QSharedPointer<Commit>, QSharedPointer<Reference>> dataByCommit;
    void fill();
};
}
