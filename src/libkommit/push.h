/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QScopedPointer>

#include <Kommit/RemoteCallbacks>
#include <Kommit/Repository>

namespace Git
{

class PushPrivate;
class Push : public QObject
{
    Q_OBJECT
public:
    enum class RemoteRedirect {
        None = GIT_REMOTE_REDIRECT_NONE,
        Initial = GIT_REMOTE_REDIRECT_INITIAL,
        All = GIT_REMOTE_REDIRECT_ALL
    };

    explicit Push(Repository *repo, QObject *parent = nullptr);

    int packBuilderPrallelism() const;
    void setPackBuilderPrallelism(const int &packBuilderPrallelism);

    Remote remote() const;
    void setRemote(const Remote &remote);

    const Git::RemoteCallbacks *remoteCallbacks() const;

    bool run();

private:
    QScopedPointer<PushPrivate> d;
};

}
