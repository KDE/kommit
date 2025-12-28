/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <Kommit/Branch>

#include <QObject>
#include <git2/remote.h>

#include "types.h"

namespace Git
{

class RemoteCallbacks;
class Repository;
class FetchOptionsPrivate;
class Proxy;
class LIBKOMMIT_EXPORT FetchOptions : public QObject
{
    Q_OBJECT
public:
    explicit FetchOptions(Repository *parent = nullptr);

    enum class DownloadTags {
        Unspecified = GIT_REMOTE_DOWNLOAD_TAGS_UNSPECIFIED,
        Auto = GIT_REMOTE_DOWNLOAD_TAGS_AUTO,
        None = GIT_REMOTE_DOWNLOAD_TAGS_NONE,
        All = GIT_REMOTE_DOWNLOAD_TAGS_ALL
    };

    enum class Prune {
        Unspecified = GIT_FETCH_PRUNE_UNSPECIFIED,
        True = GIT_FETCH_PRUNE,
        NoPrune = GIT_FETCH_NO_PRUNE
    };

    enum class UpdateFlag {
        Fetchhead = GIT_REMOTE_UPDATE_FETCHHEAD,
        ReportUnchanged = GIT_REMOTE_UPDATE_REPORT_UNCHANGED
    };
    Q_DECLARE_FLAGS(UpdateFlags, UpdateFlag)

    void apply(git_fetch_options *opts) const;

    [[nodiscard]] const Remote &remote() const;
    void setRemote(const Remote &remote);

    [[nodiscard]] Redirect redirect() const;
    void setRedirect(Redirect redirect);

    [[nodiscard]] int depth() const;
    void setDepth(int depth);

    [[nodiscard]] UpdateFlags updateFlags() const;
    void setUpdateFlags(const UpdateFlags &updateFlags);

    [[nodiscard]] RemoteCallbacks *remoteCallbacks() const;
    [[nodiscard]] Proxy *proxy() const;

    [[nodiscard]] DownloadTags downloadTags() const;
    void setDownloadTags(DownloadTags downloadTags);

    [[nodiscard]] Prune prune() const;
    void setPrune(Prune prune);

    [[nodiscard]] Branch branch() const;
    void setBranch(const Branch &branch);

Q_SIGNALS:
    void finished(bool success);

private:
    QSharedPointer<FetchOptionsPrivate> d;
};
}
