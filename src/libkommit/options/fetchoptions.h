/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QObject>
#include <git2/remote.h>

#include "types.h"

namespace Git
{

class RemoteCallbacks;
class Repository;
class FetchOptionsPrivate;
class LIBKOMMIT_EXPORT FetchOptions : public QObject
{
public:
    explicit FetchOptions(QObject *parent = nullptr);

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

    enum class UpdateFlag { Fetchhead = GIT_REMOTE_UPDATE_FETCHHEAD, ReportUnchanged = GIT_REMOTE_UPDATE_REPORT_UNCHANGED };
    Q_DECLARE_FLAGS(UpdateFlags, UpdateFlag)

    void apply(git_fetch_options *opts) const;

    [[nodiscard]] Redirect redirect() const;
    void setRedirect(Redirect redirect);

    [[nodiscard]] int depth() const;
    void setDepth(int depth);

    [[nodiscard]] UpdateFlags updateFlags() const;
    void setUpdateFlags(const UpdateFlags &updateFlags);

    [[nodiscard]] RemoteCallbacks *remoteCallbacks() const;
    void setRemoteCallbacks(RemoteCallbacks *remoteCallbacks);

    [[nodiscard]] DownloadTags downloadTags() const;
    void setDownloadTags(DownloadTags downloadTags);

    [[nodiscard]] Prune prune() const;
    void setPrune(Prune prune);

private:
    QSharedPointer<FetchOptionsPrivate> d;
};
}
