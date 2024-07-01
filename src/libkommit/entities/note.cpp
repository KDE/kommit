/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "note.h"

#include "oid.h"
#include "signature.h"

#include <git2/notes.h>

namespace Git
{

Note::Note(git_note *note)
    : mNote{note}

{
    mAuthor.reset(new Signature{git_note_author(note)});
    mCommitter.reset(new Signature{git_note_committer(note)});
    mMesage = git_note_message(note);
}

QSharedPointer<Signature> Note::author() const
{
    return mAuthor;
}

QSharedPointer<Signature> Note::committer() const
{
    return mCommitter;
}

QString Note::mesage() const
{
    return mMesage;
}

QSharedPointer<Oid> Note::oid() const
{
    return QSharedPointer<Oid>{new Oid{git_note_id(mNote)}};
}
}
