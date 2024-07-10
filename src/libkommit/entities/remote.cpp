/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/remote.h"
#include "caches/branchescache.h"
#include "entities/branch.h"
#include "gitmanager.h"
#include "types.h"

#include <git2/buffer.h>

#include <KLocalizedString>
#include <QRegularExpression>

namespace Git
{

RefSpec::RefSpec(const git_refspec *refspecs)
{
    mName = git_refspec_string(refspecs);
    mDestionation = git_refspec_dst(refspecs);
    mSource = git_refspec_src(refspecs);
    mDirection = static_cast<Direction>(git_refspec_direction(refspecs));
}

RefSpec::~RefSpec()
{
}

QString RefSpec::name() const
{
    return mName;
}

RefSpec::Direction RefSpec::direction() const
{
    return mDirection;
}

QString RefSpec::destionation() const
{
    return mDestionation;
}

QString RefSpec::source() const
{
    return mSource;
}

// Remote::Remote()
//     : mRemotePtr{nullptr}
// {
// }

Remote::Remote(git_remote *remote)
    : mRemotePtr{remote}
{
    mName = git_remote_name(remote);
    mConnected = git_remote_connected(remote);
    auto buf = git_buf{0};
    git_remote_default_branch(&buf, remote);
    mDefaultBranch = convertToQString(&buf);
    git_buf_dispose(&buf);

    mPushUrl = git_remote_pushurl(remote);
    mFetchUrl = git_remote_url(remote);

    git_strarray a1, a2;
    git_remote_get_fetch_refspecs(&a1, remote);
    git_remote_get_push_refspecs(&a2, remote);
    auto refCount = git_remote_refspec_count(remote);
    for (size_t i = 0; i < refCount; ++i) {
        auto ref = git_remote_get_refspec(remote, i);
        auto refSpec = new RefSpec{ref};
        mRefSpecList << refSpec;
    }
}

Remote::~Remote()
{
    git_remote_free(mRemotePtr);
    qDeleteAll(mRefSpecList);
}

QString Remote::name() const
{
    return mName;
}

QList<RefSpec *> Remote::refSpecList() const
{
    return mRefSpecList;
}

QString Remote::pushUrl() const
{
    return mPushUrl;
}

QString Remote::fetchUrl() const
{
    return mFetchUrl;
}

QString Remote::defaultBranch() const
{
    return mDefaultBranch;
}

const QList<QSharedPointer<Branch>> &Remote::branches()
{
    if (!mBranches.size()) {
        auto owner = Manager::owner(git_remote_owner(mRemotePtr));

        auto branches = owner->branches()->allBranches();
        for (auto const &branch : branches)
            if (branch->remoteName() == name())
                mBranches << branch;
    }
    return mBranches;
}

bool Remote::connected() const
{
    return mConnected;
}

git_remote *Remote::remotePtr() const
{
    return mRemotePtr;
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
