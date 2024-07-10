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
#include "libkommit_export.h"

namespace Git
{
class Reference;
class Commit;
class Remote;

class LIBKOMMIT_EXPORT ReferenceCache : public Cache<Reference, git_reference>
{
public:
    ReferenceCache(Manager *parent);

    DataMember findForNote(QSharedPointer<Note> note);
    DataMember findForBranch(QSharedPointer<Branch> branch);
    DataMember findForTag(QSharedPointer<Tag> tag);
    DataMember findForRemote(QSharedPointer<Remote> remote);
    DataMember findForCommit(QSharedPointer<Commit> commit);

private:
    void fill();
};

}
