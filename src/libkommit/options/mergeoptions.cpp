/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergeoptions.h"

namespace Git
{

class MergeOptionsPrivate
{
    MergeOptions *parent;

public:
    explicit MergeOptionsPrivate(MergeOptions *parent);

    MergeOptions::Flags flags{MergeOptions::Flag::Default};
    MergeOptions::MergeFileFlags mergeFileFlags{MergeOptions::MergeFileFlag::Default};
    MergeOptions::FileFlavor fileFlavor{MergeOptions::FileFlavor::Normal};
    unsigned int renameThreshold{50};
    unsigned int targetLimit{200};
    unsigned int recursionLimit{-1};
};

MergeOptionsPrivate::MergeOptionsPrivate(MergeOptions *parent)
    : parent{parent}
{
}

MergeOptions::MergeOptions()
    : d{new MergeOptionsPrivate{this}}
{
}

MergeOptions::~MergeOptions()
{
}

void MergeOptions::apply(git_merge_options *opts)
{
    opts->flags = static_cast<uint32>(d->flags);
    opts->file_flags = static_cast<uint32>(d->mergeFileFlags);
    opts->file_favor = static_cast<git_merge_file_favor_t>(d->fileFlavor);
    opts->rename_threshold = d->renameThreshold;
}

Flags MergeOptions::flags() const
{
    return d->flags;
}

void MergeOptions::setFlags(const Flags &flags)
{
    d->flags = flags;
}

MergeOptions::MergeFileFlag MergeOptions::mergeFileFlags() const
{
    return d->mergeFileFlags;
}

void MergeOptions::setMergeFileFlags(const MergeFileFlag &flags)
{
    d->mergeFileFlags = flags;
}

FileFlavor MergeOptions::fileFlavor() const
{
    return d->fileFlavor;
}

void MergeOptions::setFileFlavor(FileFlavor fileFlavor)
{
    d->fileFlavor = fileFlavor;
}

unsigned int MergeOptions::renameThreshold() const
{
    return d->renameThreshold;
}

void MergeOptions::setRenameThreshold(unsigned int renameThreshold)
{
    d->renameThreshold = renameThreshold;
}

unsigned int MergeOptions::targetLimit() const
{
    return d->targetLimit;
}

void MergeOptions::setTargetLimit(const unsigned int &targetLimit)
{
    d->targetLimit = targetLimit;
}

unsigned int MergeOptions::recursionLimit() const
{
    return d->recursionLimit;
}

void MergeOptions::setRecursionLimit(const unsigned int &recursionLimit)
{
    d->recursionLimit = recursionLimit;
}
}
