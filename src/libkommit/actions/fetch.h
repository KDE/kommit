/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QScopedPointer>
#include <QSharedPointer>

#include <git2/remote.h>

#include "abstractaction.h"
#include "libkommit_export.h"
#include "types.h"

namespace Git
{

class Certificate;
class Branch;
class Remote;
class FetchPrivate;
class Repository;
class Proxy;
struct PackProgress;
struct FetchTransferStat;
class RemoteCallbacks;

class LIBKOMMIT_EXPORT Fetch : public AbstractAction
{
    Q_OBJECT
public:
    enum class Prune { PruneUnspecified = GIT_FETCH_PRUNE_UNSPECIFIED, Prune = GIT_FETCH_PRUNE, NoPrune = GIT_FETCH_NO_PRUNE };
    Q_ENUM(Prune)

    enum class DownloadTags {
        Unspecified = GIT_REMOTE_DOWNLOAD_TAGS_UNSPECIFIED,
        Auto = GIT_REMOTE_DOWNLOAD_TAGS_AUTO,
        None = GIT_REMOTE_DOWNLOAD_TAGS_NONE,
        All = GIT_REMOTE_DOWNLOAD_TAGS_ALL
    };
    Q_ENUM(DownloadTags)

    enum class AcceptCertificate { None, OnlyValid, All };
    Q_ENUM(AcceptCertificate)

    explicit Fetch(Repository *repo, QObject *parent = nullptr);
    ~Fetch();

    [[nodiscard]] const Remote &remote() const;
    void setRemote(const Remote &remote);

    [[nodiscard]] Prune prune() const;
    void setPrune(Prune prune);

    [[nodiscard]] DownloadTags downloadTags() const;
    void setDownloadTags(DownloadTags downloadTags);

    [[nodiscard]] int depth() const;
    void setDepth(int depth);

    [[nodiscard]] Redirect redirect() const;
    void setRedirect(Redirect redirect);

    [[nodiscard]] QStringList customHeaders() const;
    void setCustomHeaders(const QStringList &customHeaders);

    [[nodiscard]] const Branch &branch() const;
    void setBranch(const Branch &branch);

    [[nodiscard]] AcceptCertificate acceptCertificate() const;
    void setAcceptCertificate(AcceptCertificate acceptCertificate);

    [[nodiscard]] const RemoteCallbacks *remoteCallbacks() const;
    [[nodiscard]] const Proxy *proxy() const;

protected:
    int exec() override;

private:
    QScopedPointer<FetchPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Fetch)
};

}
