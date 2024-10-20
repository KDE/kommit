/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "blamehunk.h"

#include "caches/commitscache.h"
#include "repository.h"

namespace Git
{

class BlameHunkPrivate
{
public:
    explicit BlameHunkPrivate(Repository *git, const git_blame_hunk *blameHunk = nullptr);

    const git_blame_hunk *blameHunk{nullptr};

    Oid commitId;

    size_t startLine;
    size_t linesCount;

    QString originPath;

    Commit finalCommit;
    Commit originCommit;

    Signature finalSignature;
    Signature originSignature;

    size_t finalStartLineNumber;
    size_t originStartLineNumber;
};

BlameHunkPrivate::BlameHunkPrivate(Repository *git, const git_blame_hunk *hunk)
    : blameHunk{hunk}
{
    if (blameHunk) {
        commitId = Oid{hunk->final_commit_id};
        startLine = hunk->final_start_line_number;
        linesCount = hunk->lines_in_hunk;
        originPath = hunk->orig_path;
        finalCommit = git->commits()->findByOid(&hunk->final_commit_id);
        originCommit = git->commits()->findByOid(&hunk->orig_commit_id);
        finalSignature = Signature{hunk->orig_signature};
        originSignature = Signature{hunk->final_signature};
        finalStartLineNumber = hunk->final_start_line_number;
        originStartLineNumber = hunk->orig_start_line_number;
    }
}

BlameHunk::BlameHunk(Repository *git, const git_blame_hunk *hunk)
    : d{new BlameHunkPrivate{git, hunk}}
{
}

BlameHunk::BlameHunk(const BlameHunk &other)
    : d{other.d}
{
}

BlameHunk &BlameHunk::operator=(const BlameHunk &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool BlameHunk::operator==(const BlameHunk &other) const
{
    return d->blameHunk == other.d->blameHunk;
}

bool BlameHunk::operator!=(const BlameHunk &other) const
{
    return !(*this == other);
}

bool BlameHunk::isNull() const
{
    return !d->blameHunk;
}

const git_blame_hunk *BlameHunk::constData() const
{
    return d->blameHunk;
}

size_t BlameHunk::startLine() const
{
    return d->startLine;
}

size_t BlameHunk::linesCount() const
{
    return d->linesCount;
}

size_t BlameHunk::finalStartLineNumber() const
{
    return d->finalStartLineNumber;
}

size_t BlameHunk::originStartLineNumber() const
{
    return d->originStartLineNumber;
}

const Oid &BlameHunk::commitId() const
{
    return d->commitId;
}

const QString &BlameHunk::originPath() const
{
    return d->originPath;
}

const Commit &BlameHunk::finalCommit() const
{
    return d->finalCommit;
}

const Commit &BlameHunk::originCommit() const
{
    return d->originCommit;
}

const Signature &BlameHunk::finalSignature() const
{
    return d->finalSignature;
}

const Signature &BlameHunk::originSignature() const
{
    return d->originSignature;
}
}
