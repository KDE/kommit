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

class Note;

class LIBKOMMIT_EXPORT NotesCache : public Cache<git_note, Note>
{
public:
    explicit NotesCache(Manager *parent);
};
};
