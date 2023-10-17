/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchoptions.h"

namespace Git
{

FetchOptions::FetchOptions()
{
}

void FetchOptions::applyToFetchOptions(git_fetch_options *opts) const
{
    opts->download_tags = static_cast<git_remote_autotag_option_t>(downloadTags);
    opts->prune = static_cast<git_fetch_prune_t>(prune);
}

}
