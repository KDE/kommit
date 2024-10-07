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
    BlameOptionsPrivate(BlameOptions *parent);

    QSharedPointer<Commit> firstCommit;
    QSharedPointer<Commit> lastCommit;
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
    : d_ptr{new BlameOptionsPrivate{this}}
{
}
QSharedPointer<Commit> BlameOptions::firstCommit() const
{
    Q_D(const BlameOptions);
    return d->firstCommit;
}

void BlameOptions::setFirstCommit(QSharedPointer<Commit> firstCommit)
{
    Q_D(BlameOptions);
    d->firstCommit = firstCommit;
}

QSharedPointer<Commit> BlameOptions::lastCommit() const
{
    Q_D(const BlameOptions);
    return d->lastCommit;
}

void BlameOptions::setLastCommit(QSharedPointer<Commit> lastCommit)
{
    Q_D(BlameOptions);
    d->lastCommit = lastCommit;
}

int BlameOptions::minMatchCharacters() const
{
    Q_D(const BlameOptions);
    return d->minMatchCharacters;
}

void BlameOptions::setMinMatchCharacters(int minMatchCharacters)
{
    Q_D(BlameOptions);
    d->minMatchCharacters = minMatchCharacters;
}

int BlameOptions::firstLineNumber() const
{
    Q_D(const BlameOptions);
    return d->firstLineNumber;
}

void BlameOptions::setFirstLineNumber(int firstLineNumber)
{
    Q_D(BlameOptions);
    d->firstLineNumber = firstLineNumber;
}

int BlameOptions::lastLineNumber() const
{
    Q_D(const BlameOptions);
    return d->lastLineNumber;
}

void BlameOptions::setLastLineNumber(int lastLineNumber)
{
    Q_D(BlameOptions);
    d->lastLineNumber = lastLineNumber;
}

void BlameOptions::apply(git_blame_options *opts)
{
    Q_D(BlameOptions);
    if (d->minMatchCharacters != -1)
        opts->min_match_characters = d->minMatchCharacters;
    if (d->firstLineNumber != -1)
        opts->min_line = d->firstLineNumber;
    if (d->lastLineNumber != -1)
        opts->max_line = d->firstLineNumber;

    if (!d->firstCommit.isNull()) {
        git_oid firstCommit;

        d->firstCommit->oid()->copyTo(&firstCommit);
        opts->oldest_commit = firstCommit;
    }
    if (!d->lastCommit.isNull()) {
        git_oid lastCommit;

        d->lastCommit->oid()->copyTo(&lastCommit);
        opts->oldest_commit = lastCommit;
    }
    opts->flags = d->flags;
}

BlameOptions::BlameFlags BlameOptions::flags() const
{
    Q_D(const BlameOptions);
    return d->flags;
}

void BlameOptions::setFlags(const BlameFlags &flags)
{
    Q_D(BlameOptions);
    d->flags = flags;
}

}
