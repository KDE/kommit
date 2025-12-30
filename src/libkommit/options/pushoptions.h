/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>

#include <git2/remote.h>

#include "types.h"
#include <Kommit/Branch>
#include <Kommit/Remote>

#include "libkommit_export.h"

namespace Git
{

class RemoteCallbacks;
class Repository;
class PushOptionsPrivate;
class LIBKOMMIT_EXPORT PushOptions
{
    Q_GADGET

public:
    explicit PushOptions(Repository *repo);
    ~PushOptions();

    enum { PackBuilderParallelJobsAutoDetect = 0 };

    [[nodiscard]] Remote remote() const;
    void setRemote(const Remote &remote);

    [[nodiscard]] Branch branch() const;
    void setBranch(const Branch &branch);

    [[nodiscard]] RemoteCallbacks *remoteCallbacks() const;

    [[nodiscard]] Redirect redirect() const;
    void setRedirect(Redirect redirect);

    [[nodiscard]] QStringList customHeaders() const;
    void setCustomHeaders(const QStringList &customHeaders);

    [[nodiscard]] QStringList remotePushOptions() const;
    void setRemotePushOptions(const QStringList &remotePushOptions);

    [[nodiscard]] int packBuilderParallelJobs() const;
    void setPackBuilderParallelJobs(int packBuilderParallelJobs);

    void apply(git_push_options *opts);

private:
    QScopedPointer<PushOptionsPrivate> d;
};

}
