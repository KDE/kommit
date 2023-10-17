/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include <git2/remote.h>

namespace Git
{

class LIBKOMMIT_EXPORT FetchOptions
{
public:
    FetchOptions();

    enum class DownloadTags {
        Unspecified = 0,
        Auto,
        None,
        All,
    };

    enum class Prune {
        Unspecified = 0,
        Prune,
        NoPrune,
    };

    DownloadTags downloadTags{DownloadTags::Unspecified};
    Prune prune{Prune::Unspecified};
    void applyToFetchOptions(git_fetch_options *opts) const;
};

}
