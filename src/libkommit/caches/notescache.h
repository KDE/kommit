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
#include "entities/note.h"
#include "libkommit_export.h"

namespace Git
{

class Oid;

class LIBKOMMIT_EXPORT NotesCache : public Cache<Note, git_note>
{
public:
    explicit NotesCache(Manager *parent);

    [[nodiscard]] ListType allNotes();
    [[nodiscard]] DataType findByOid(const git_oid *oid);

protected:
    void clearChildData() override;
};
};
