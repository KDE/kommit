/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "filemode.h"

#include <git2/diff.h>

#include <QScopedPointer>
#include <QString>
#include <oid.h>

namespace Git
{

struct DiffFile {
    enum class Flag {
        Binary = GIT_DIFF_FLAG_BINARY,
        NotBinary = GIT_DIFF_FLAG_NOT_BINARY,
        ValidId = GIT_DIFF_FLAG_VALID_ID,
        Exists = GIT_DIFF_FLAG_EXISTS,
        ValidSize = GIT_DIFF_FLAG_VALID_SIZE
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    explicit DiffFile(git_diff_file *file);
    explicit DiffFile(const git_diff_file *file);

    Oid oid;
    QString path;
    Flags flags;
    FileMode mode;
    uint64_t size;
};

class FileDeltaPrivate;
class FileDelta
{
public:
    enum class Flag {
        Binary = GIT_DIFF_FLAG_BINARY,
        NotBinary = GIT_DIFF_FLAG_NOT_BINARY,
        ValidId = GIT_DIFF_FLAG_VALID_ID,
        Exists = GIT_DIFF_FLAG_EXISTS,
        ValidSize = GIT_DIFF_FLAG_VALID_SIZE,
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    explicit FileDelta(git_diff_delta *delta);
    explicit FileDelta(const git_diff_delta *delta);
    ~FileDelta();

private:
    QScopedPointer<FileDeltaPrivate> d_ptr;
    Q_DECLARE_PRIVATE(FileDelta)
};

}
