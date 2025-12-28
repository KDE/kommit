/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diffdelta.h"

namespace Git
{

DiffFile::DiffFile(const git_diff_file *file)
    : d{file}
{
}

Oid DiffFile::id() const
{
    if (!d)
        return {};
    return Oid{d->id};
}

QString DiffFile::path() const
{
    if (!d)
        return {};
    return QString{d->path};
}

qsizetype DiffFile::size() const
{
    if (!d)
        return {};
    return d->size;
}

DiffFile::Flags DiffFile::flags() const
{
    if (!d)
        return Flag::Binary;
    return static_cast<Flags>(d->flags);
}

FileMode DiffFile::mode() const
{
    if (!d)
        return FileMode{};
    return FileMode{d->mode};
}

bool DiffFile::isNull() const
{
    return !d;
}

DiffDelta::DiffDelta(const git_diff_delta *delta)
    : d{delta}
{
}

DeltaFlag DiffDelta::status() const
{
    if (!d)
        return DeltaFlag::Unreadable;

    return static_cast<DeltaFlag>(d->status);
}

quint16 DiffDelta::similarity() const
{
    if (!d)
        return 0;
    return d->similarity;
}

quint16 DiffDelta::finesCount() const
{
    if (!d)
        return 0;
    return d->nfiles;
}

DiffFile DiffDelta::oldFile() const
{
    if (!d)
        return DiffFile{nullptr};
    return DiffFile{&d->old_file};
}

DiffFile DiffDelta::newFile() const
{
    if (!d)
        return DiffFile{nullptr};
    return DiffFile{&d->new_file};
}

bool DiffDelta::isNull() const
{
    return !d;
}

DiffDelta::Flags DiffDelta::flags() const
{
    if (!d)
        return Flag::Binary;

    return static_cast<Flags>(d->flags);
}

}
