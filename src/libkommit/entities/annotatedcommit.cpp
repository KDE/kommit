/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "annotatedcommit.h"

#include <git2/annotated_commit.h>

#include <Kommit/Commit>

namespace Git
{

class AnnotatedCommitPrivate
{
public:
    git_annotated_commit *annotatedCommit;
};

AnnotatedCommit::AnnotatedCommit()
    : d{new AnnotatedCommitPrivate}
{
}

AnnotatedCommit::AnnotatedCommit(git_oid *oid)
    : d{new AnnotatedCommitPrivate}
{
    // git_annotated_commit_lookup()
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
    git_annotated_commit_ref(d->annotatedCommit);
    return {}; // TODO
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
