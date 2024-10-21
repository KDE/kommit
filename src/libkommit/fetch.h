/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>

#include <git2/remote.h>

#include "libkommit_export.h"

namespace Git
{

class Certificate;
class Credential;
class Branch;
class Remote;
class FetchPrivate;
class Repository;
class Reference;
class Oid;
struct PackProgress;
struct FetchTransferStat;
class RemoteCallbacks;

class LIBKOMMIT_EXPORT Fetch : public QObject
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

    enum class Redirect { None = GIT_REMOTE_REDIRECT_NONE, Initial = GIT_REMOTE_REDIRECT_INITIAL, All = GIT_REMOTE_REDIRECT_ALL };
    Q_ENUM(Redirect)

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

    bool run();
    void runAsync();

Q_SIGNALS:
    void finished(bool success);

private:
    QScopedPointer<FetchPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Fetch)
};

}
