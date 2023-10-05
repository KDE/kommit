/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "note.h"

#include <git2/notes.h>

namespace Git
{

Note::Note(git_note *note)
    : mNote{note}
    , mAuthor{Signature{git_note_author(note)}}
    , mCommitter{Signature{git_note_committer(note)}}

{
    mMesage = git_note_message(note);
}

Signature Note::author() const
{
    return mAuthor;
}

Signature Note::committer() const
{
    return mCommitter;
}

QString Note::mesage() const
{
    return mMesage;
}

}
