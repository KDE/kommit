/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/blame.h>

#include <Kommit/Commit>
#include <Kommit/Oid>
#include <Kommit/Signature>

#include <QSharedPointer>

#include "libkommit_export.h"

namespace Git
{

class BlameHunkPrivate;
class LIBKOMMIT_EXPORT BlameHunk
{
public:
    BlameHunk(Repository *git, const git_blame_hunk *hunk);
    BlameHunk(const BlameHunk &other);
    BlameHunk &operator=(const BlameHunk &other);
    bool operator==(const BlameHunk &other) const;
    bool operator!=(const BlameHunk &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] const git_blame_hunk *constData() const;

    size_t startLine() const;
    size_t linesCount() const;
    size_t finalStartLineNumber() const;
    size_t originStartLineNumber() const;

    const Oid &commitId() const;

    const QString &originPath() const;

    const Commit &finalCommit() const;
    const Commit &originCommit() const;

    const Signature &finalSignature() const;
    const Signature &originSignature() const;

private:
    QSharedPointer<BlameHunkPrivate> d;
};

}
