/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libkommit_export.h"

#include <QList>
#include <QString>

#include <git2/remote.h>

#include <QSharedPointer>

#include <Kommit/RefSpec>

namespace Git
{

class Branch;
class LIBKOMMIT_EXPORT RemoteBranch
{
public:
    bool configuredPull{false};
    bool configuredPush{false};

    QString name;
    QString remotePushBranch;
    QString remotePullBranch;
    enum class Status { Unknown, UpToDate, FastForwardable, LocalOutOfDate };
    Status status{Status::Unknown};

    [[nodiscard]] QString statusText() const;
};

class RemotePrivate;
class LIBKOMMIT_EXPORT Remote
{
public:
    Remote();
    explicit Remote(git_remote *remote);

    Remote(const Remote &other);
    Remote &operator=(const Remote &other);
    bool operator==(const Remote &other) const;
    bool operator!=(const Remote &other) const;

    [[nodiscard]] QString name() const;
    [[nodiscard]] QList<RefSpec> refSpecList() const;
    [[nodiscard]] QString pushUrl() const;
    [[nodiscard]] QString fetchUrl() const;
    [[nodiscard]] QString defaultBranch() const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] const QList<Branch> &branches();

    [[nodiscard]] bool connected() const;

    [[nodiscard]] git_remote *remotePtr() const;

private:
    QSharedPointer<RemotePrivate> d;

    friend class RemotesModel;
};

} // namespace Git
