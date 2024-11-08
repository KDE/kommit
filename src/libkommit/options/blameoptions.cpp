/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "blameoptions.h"

#include "entities/commit.h"
#include "entities/oid.h"

namespace Git
{

class BlameOptionsPrivate
{
    BlameOptions *q_ptr;
    Q_DECLARE_PUBLIC(BlameOptions)

public:
    explicit BlameOptionsPrivate(BlameOptions *parent);

    Commit firstCommit;
    Commit lastCommit;
    int minMatchCharacters{-1};
    int firstLineNumber{-1};
    int lastLineNumber{-1};
    BlameOptions::BlameFlags flags{BlameOptions::BlameFlag::Normal};
};
BlameOptionsPrivate::BlameOptionsPrivate(BlameOptions *parent)
    : q_ptr{parent}
{
}
BlameOptions::BlameOptions()
    : d{new BlameOptionsPrivate{this}}
{
}

BlameOptions::~BlameOptions()
{
}

Commit BlameOptions::firstCommit() const
{
    return d->firstCommit;
}

void BlameOptions::setFirstCommit(const Commit &firstCommit)
{
    d->firstCommit = firstCommit;
}

Commit BlameOptions::lastCommit() const
{
    return d->lastCommit;
}

void BlameOptions::setLastCommit(const Commit &lastCommit)
{
    d->lastCommit = lastCommit;
}

int BlameOptions::minMatchCharacters() const
{
    return d->minMatchCharacters;
}

void BlameOptions::setMinMatchCharacters(int minMatchCharacters)
{
    d->minMatchCharacters = minMatchCharacters;
}

int BlameOptions::firstLineNumber() const
{
    return d->firstLineNumber;
}

void BlameOptions::setFirstLineNumber(int firstLineNumber)
{
    d->firstLineNumber = firstLineNumber;
}

int BlameOptions::lastLineNumber() const
{
    return d->lastLineNumber;
}

void BlameOptions::setLastLineNumber(int lastLineNumber)
{
    d->lastLineNumber = lastLineNumber;
}

void BlameOptions::apply(git_blame_options *opts)
{
    if (d->minMatchCharacters != -1)
        opts->min_match_characters = d->minMatchCharacters;
    if (d->firstLineNumber != -1)
        opts->min_line = d->firstLineNumber;
    if (d->lastLineNumber != -1)
        opts->max_line = d->firstLineNumber;

    if (!d->firstCommit.isNull()) {
        opts->oldest_commit = *d->firstCommit.oid().data();
    }
    if (!d->lastCommit.isNull()) {
        opts->oldest_commit = *d->lastCommit.oid().data();
    }
    opts->flags = d->flags;
}

BlameOptions::BlameFlags BlameOptions::flags() const
{
    return d->flags;
}

void BlameOptions::setFlags(const BlameFlags &flags)
{
    d->flags = flags;
}
}
