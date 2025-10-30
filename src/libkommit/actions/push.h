// /*
// SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

// SPDX-License-Identifier: GPL-3.0-or-later
// */

// #pragma once

// #include <QObject>
// #include <QSharedPointer>

// #include "abstractaction.h"
// #include "branch.h"
// #include "remote.h"
// #include "types.h"

// namespace Git
// {

// class RemoteCallbacks;
// class PushPrivate;
// class Repository;
// class LIBKOMMIT_EXPORT PushAction : public AbstractAction
// {
//     Q_OBJECT

// public:
//     explicit PushAction(Repository *repo, QObject *parent = nullptr);

//     enum { PackBuilderParallelJobsAutoDetect = 0 };

//     [[nodiscard]] Remote remote() const;
//     void setRemote(const Remote &remote);

//     [[nodiscard]] Branch branch() const;
//     void setBranch(const Branch &branch);

//     [[nodiscard]] RemoteCallbacks *remoteCallbacks() const;

//     [[nodiscard]] Redirect redirect() const;
//     void setRedirect(Redirect redirect);

//     [[nodiscard]] QStringList customHeaders() const;
//     void setCustomHeaders(const QStringList &customHeaders);

//     [[nodiscard]] QStringList remotePushOptions() const;
//     void setRemotePushOptions(const QStringList &remotePushOptions);

//     [[nodiscard]] int packBuilderParallelJobs() const;
//     void setPackBuilderParallelJobs(int packBuilderParallelJobs);

// protected:
//     int exec() override;

// private:
//     QSharedPointer<PushPrivate> d;
// };

// }
