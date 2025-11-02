/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QSharedPointer>

#include <Kommit/Reference>

namespace Git
{

class AnnotatedCommitPrivate;
class LIBKOMMIT_EXPORT AnnotatedCommit
{
public:
    AnnotatedCommit();
    AnnotatedCommit(git_oid *oid);

    [[nodiscard]] git_annotated_commit *data() const;
    [[nodiscard]] const git_annotated_commit *constData() const;

    Reference reference() const;
    Commit commit() const;
    Oid oid() const;

private:
    QSharedPointer<AnnotatedCommitPrivate> d;
};

}
