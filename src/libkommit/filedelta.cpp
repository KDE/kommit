/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filedelta.h"

namespace Git
{

class FileDeltaPrivate
{
    FileDelta *q_ptr;
    Q_DECLARE_PUBLIC(FileDelta)

public:
    FileDeltaPrivate(FileDelta *parent, git_diff_delta *delta);

    git_delta_t status;
    uint32_t flags;
    uint16_t similarity;
    uint16_t nfiles;
    DiffFile old_file;
    DiffFile new_file;
};
FileDeltaPrivate::FileDeltaPrivate(FileDelta *parent, git_diff_delta *delta)
    : q_ptr{parent}
    , status{delta->status}
    , flags{delta->flags}
    , similarity{delta->similarity}
    , nfiles{delta->nfiles}
    , old_file{DiffFile{&delta->old_file}}
    , new_file{DiffFile{&delta->new_file}}
{
}

DiffFile::DiffFile(git_diff_file *file)
    : oid{file->id}
    , path{file->path}
    , flags{static_cast<Flags>(file->flags)}
    , mode{file->mode}
    , size{file->size}
{
}

FileDelta::FileDelta(git_diff_delta *delta)
    : d_ptr{new FileDeltaPrivate{this, delta}}
{
}

}
