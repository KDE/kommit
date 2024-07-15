/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "notescache.h"
#include "entities/commit.h"
#include "entities/oid.h"
#include "gitmanager.h"

#include <git2/notes.h>
#include <git2/types.h>

#include <entities/note.h>

namespace Git
{

NotesCache::NotesCache(Manager *parent)
    : Cache<Note, git_note>{parent}
{
}

NotesCache::DataList NotesCache::allNotes()
{
    struct wrapper {
        git_repository *repo;
        NotesCache *notesCache;
        DataList notes;
    };
    auto cb = [](const git_oid *blob_id, const git_oid *annotated_object_id, void *payload) -> int {
        Q_UNUSED(blob_id);

        auto w = reinterpret_cast<wrapper *>(payload);
        git_note *note;
        if (!git_note_read(&note, w->repo, NULL, annotated_object_id))
            w->notes << w->notesCache->findByPtr(note);
        return 0;
    };
    wrapper w;
    w.notesCache = this;
    w.repo = manager->repoPtr();
    git_note_foreach(manager->repoPtr(), NULL, cb, &w);
    return w.notes;
}

NotesCache::DataMember NotesCache::findByOid(const git_oid *oid)
{
    git_note *note;
    if (git_note_read(&note, manager->repoPtr(), NULL, oid))
        return {};

    return findByPtr(note);
}

void NotesCache::clearChildData()
{
}
}
