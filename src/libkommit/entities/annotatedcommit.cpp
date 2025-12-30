/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "annotatedcommit.h"

#include <git2/annotated_commit.h>
#include <git2/refs.h>

#include <Kommit/Commit>
#include <Kommit/Repository>

namespace Git
{

class AnnotatedCommitPrivate
{
public:
    AnnotatedCommitPrivate(git_annotated_commit *commit)
        : annotatedCommit{commit}
    {
    }

    ~AnnotatedCommitPrivate()
    {
        git_annotated_commit_free(annotatedCommit);
    }

    git_annotated_commit *annotatedCommit{nullptr};
};

AnnotatedCommit::AnnotatedCommit(git_annotated_commit *commit)
    : d{new AnnotatedCommitPrivate{commit}}
{
}

git_annotated_commit *AnnotatedCommit::data() const
{
    return d->annotatedCommit;
}

const git_annotated_commit *AnnotatedCommit::constData() const
{
    return d->annotatedCommit;
}

Reference AnnotatedCommit::reference() const
{
    auto refName = git_annotated_commit_ref(d->annotatedCommit);
    // git_reference_owner()
    git_reference *ref;
    if (git_reference_lookup(&ref, nullptr, refName))
        return {};
    return Reference{ref};
}

Commit AnnotatedCommit::commit() const
{
    return {};
}

Oid AnnotatedCommit::oid() const
{
    return Oid{git_annotated_commit_id(d->annotatedCommit)};
}

}
