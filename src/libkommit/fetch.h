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

    Fetch(Repository *repo, QObject *parent = nullptr);
    ~Fetch();

    [[nodiscard]] QSharedPointer<Remote> remote() const;
    void setRemote(QSharedPointer<Remote> remote);

    [[nodiscard]] Prune prune() const;
    void setPrune(Prune prune);

    [[nodiscard]] DownloadTags downloadTags() const;
    void setDownloadTags(DownloadTags downloadTags);

    [[nodiscard]] int depth() const;
    void setDepth(int depth);

    [[nodiscard]] Redirect redirect() const;
    void setRedirect(Redirect redirect);

    void run();

    [[nodiscard]] QStringList customHeaders() const;
    void setCustomHeaders(const QStringList &customHeaders);

    [[nodiscard]] QSharedPointer<Branch> branch() const;
    void setBranch(QSharedPointer<Branch> branch);

Q_SIGNALS:
    void message(const QString &message);
    void credentialRequeted(const QString &url, Credential *cred);
    void transferProgress(const FetchTransferStat *stat);
    void packProgress(const PackProgress *p);
    void updateRef(QSharedPointer<Reference> reference, QSharedPointer<Oid> a, QSharedPointer<Oid> b);
    void certificateCheck(Certificate *cert, bool *accept);
    void finished();

private:
    QScopedPointer<FetchPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Fetch)
};

}
