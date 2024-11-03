/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include <QObject>
#include <git2/remote.h>

namespace Git
{

class Repository;
class LIBKOMMIT_EXPORT FetchOptions
{
public:
    FetchOptions(QObject *parent = nullptr);

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
    void apply(git_remote_callbacks *callbacks, Repository *repo);
};
}
