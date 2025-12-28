/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include <git2/diff.h>

#include <types.h>

#include <Kommit/FileDelta>
#include <Kommit/FileMode>
#include <Kommit/Oid>

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

    explicit DiffFile(const git_diff_file *file);

    Oid id() const;
    QString path() const;
    qsizetype size() const;
    Flags flags() const;
    FileMode mode() const;

    bool isNull() const;

private:
    const git_diff_file *d;
};

class DiffDelta
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

    explicit DiffDelta(const git_diff_delta *delta);

    DeltaFlag status() const;
    Flags flags() const;
    quint16 similarity() const;
    quint16 finesCount() const;
    DiffFile oldFile() const;
    DiffFile newFile() const;

    bool isNull() const;

private:
    const git_diff_delta *d;
};

}
