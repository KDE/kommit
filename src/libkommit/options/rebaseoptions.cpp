/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "rebaseoptions.h"

namespace Git
{

class RebaseOptionsPrivate
{
    RebaseOptions *parent;

public:
    RebaseOptionsPrivate(RebaseOptions *parent);
};

RebaseOptionsPrivate::RebaseOptionsPrivate(RebaseOptions *parent)
    : parent{parent}
{
}
RebaseOptions::RebaseOptions()
    : d{new RebaseOptionsPrivate{this}}
{
}

RebaseOptions::~RebaseOptions()
{
}

void RebaseOptions::apply(git_rebase_options *opts)
{
    // apply
}

}
