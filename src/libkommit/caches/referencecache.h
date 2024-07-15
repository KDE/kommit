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
#include "entities/reference.h"
#include "libkommit_export.h"

namespace Git
{

class Commit;
class Remote;

class ReferenceCachePrivate;
class LIBKOMMIT_EXPORT ReferenceCache : public Cache<Reference, git_reference>
{
public:
    ReferenceCache(Manager *parent);
    virtual ~ReferenceCache();

    DataMember findByName(const QString &name);
    DataMember findForNote(QSharedPointer<Note> note);
    DataMember findForBranch(QSharedPointer<Branch> branch);
    DataMember findForTag(QSharedPointer<Tag> tag);
    DataMember findForRemote(QSharedPointer<Remote> remote);
    DataList findForCommit(QSharedPointer<Commit> commit);

    void forEach(std::function<void(DataMember)> callback) const;

protected:
    void clearChildData() override;

private:
    void fill();

    ReferenceCachePrivate *d_ptr;
    Q_DECLARE_PRIVATE(ReferenceCache)
};

}
