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

class NotePrivate
{
public:
    explicit NotePrivate(git_note *note);
    ~NotePrivate();
    git_note *note;
};
NotePrivate::NotePrivate(git_note *note)
    : note{note}
{
}

NotePrivate::~NotePrivate()
{
    git_note_free(note);
}

Note::Note(git_note *note)
    : d{new NotePrivate{note}}

{
}

Note::Note(const Note &other)
    : d{other.d}
{
}

Note &Note::operator=(const Note &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

Signature Note::author() const
{
    return Signature{git_note_author(d->note)};
}

Signature Note::committer() const
{
    return Signature{git_note_committer(d->note)};
}

QString Note::message() const
{
    return QString{git_note_message(d->note)};
}

Oid Note::oid() const
{
    return Oid{git_note_id(d->note)};
}

bool Note::isNull() const
{
    return !d->note;
}

bool Note::operator==(const Note &other) const
{
    return d->note == other.d->note;
}

bool Note::operator!=(const Note &other) const
{
    return !(*this == other);
}
}
