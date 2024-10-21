/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/remote.h"
#include "branch.h"
#include "buffer.h"
#include "caches/branchescache.h"
#include "entities/branch.h"
#include "gitglobal_p.h"
#include "remotecallbacks.h"
#include "repository.h"
#include "types.h"

#include <git2/buffer.h>

#include <KLocalizedString>
#include <QRegularExpression>

namespace Git
{

class RemotePrivate
{
public:
    explicit RemotePrivate(git_remote *remote = nullptr);
    ~RemotePrivate();
    git_remote *remote{nullptr};

    QList<RefSpec> refSpecList;
    QList<Branch> branches;
};

RemotePrivate::RemotePrivate(git_remote *remote)
    : remote{remote}
{
}

RemotePrivate::~RemotePrivate()
{
    git_remote_free(remote);
}

Remote::Remote()
    : d{new RemotePrivate{nullptr}}
{
}

Remote::Remote(git_remote *remote)
    : d{new RemotePrivate{remote}}
{
    if (remote) {
        git_strarray a1, a2;
        git_remote_get_fetch_refspecs(&a1, remote);
        git_remote_get_push_refspecs(&a2, remote);
        auto refCount = git_remote_refspec_count(remote);
        for (size_t i = 0; i < refCount; ++i) {
            auto ref = git_remote_get_refspec(remote, i);
            d->refSpecList << RefSpec{ref};
        }
    }
}

Remote::Remote(const Remote &other)
    : d{other.d}
{
}

Remote &Remote::operator=(const Remote &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Remote::operator==(const Remote &other) const
{
    return d->remote == other.d->remote;
}

bool Remote::operator!=(const Remote &other) const
{
    return !(*this == other);
}

QString Remote::name() const
{
    return QString{git_remote_name(d->remote)};
}

QList<RefSpec> Remote::refSpecList() const
{
    return d->refSpecList;
}

QString Remote::pushUrl() const
{
    return QString{git_remote_pushurl(d->remote)};
}

QString Remote::fetchUrl() const
{
    return QString{git_remote_url(d->remote)};
}

QString Remote::defaultBranch() const
{
    Buf b;

    if (SequenceRunner::runSingle(git_remote_default_branch, &b, d->remote))
        return {};

    return b.toString();
}

bool Remote::isNull() const
{
    return !d->remote;
}

const QList<Branch> &Remote::branches()
{
    if (!d->branches.size()) {
        auto owner = Repository::owner(git_remote_owner(d->remote));

        auto branches = owner->branches()->allBranches();
        for (auto const &branch : branches)
            if (branch.remoteName() == name())
                d->branches << branch;
    }
    return d->branches;
}

bool Remote::isConnected() const
{
    return git_remote_connected(d->remote);
    return 1 == SequenceRunner::runSingle(git_remote_connected, d->remote);
}

bool Remote::connect(Direction direction, RemoteCallbacks *callBacks) const
{
    git_remote_callbacks cb = GIT_REMOTE_CALLBACKS_INIT;
    if (callBacks) {
        callBacks->apply(&cb, Repository::owner(git_remote_owner(d->remote)));
    }
    return 0
        == SequenceRunner::runSingle(git_remote_connect,
                                     d->remote,
                                     static_cast<git_direction>(direction),
                                     &cb,
                                     (const git_proxy_options *)NULL,
                                     (const git_strarray *)NULL);
}

git_remote *Remote::remotePtr() const
{
    return d->remote;
}

QString RemoteBranch::statusText() const
{
    switch (status) {
    case Status::Unknown:
        return i18n("Unknown");
    case Status::UpToDate:
        return i18n("Up to date");
    case Status::FastForwardable:
        return i18n("Fast forwardable");
    case Status::LocalOutOfDate:
        return i18n("Local out of date");
    }
    return {};
}

} // namespace Git
