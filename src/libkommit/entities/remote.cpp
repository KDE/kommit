/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/remote.h"
#include "libkommit_debug.h"
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

Remote::Remote() = default;

Remote::Remote(git_remote *remote)
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

void Remote::parse(const QString &output)
{
    /*
* remote origin
  Fetch URL: http://gitlab.ehive.cloud/ai/EHive_Analyser.git
  Push  URL: http://gitlab.ehive.cloud/ai/EHive_Analyser.git
  HEAD branch: alpha
  Local branches configured for 'git pull':
    alpha             merges with remote alpha
    master            merges with remote master
    segment_init      merges with remote segment_init
    wip/analyser      merges with remote wip/analyser
    wip/video_trimmer merges with remote wip/video_trimmer
    zoom              merges with remote zoom
  Local refs configured for 'git push':
    alpha                 pushes to alpha                 (up to date)
    dev                   pushes to dev                   (fast-forwardable)
    master                pushes to master                (up to date)
    wip/analyser          pushes to wip/analyser          (up to date)
    wip/database_importer pushes to wip/database_importer (up to date)
    wip/doc               pushes to wip/doc               (up to date)
    wip/export_logs       pushes to wip/export_logs       (up to date)
    wip/video_import      pushes to wip/video_import      (up to date)
    wip/video_trimmer     pushes to wip/video_trimmer     (up to date)
*/
    //    enum ParseMode {
    //        None,
    //        GitPull,
    //        GitPush,
    //    };

    //    ParseMode mode{None};
    //    auto lines = output.split(QLatin1Char('\n'));
    //    const QRegularExpression regexPull{QStringLiteral(R"((\S+)\s+merges with remote\s+(\S+))")};
    //    const QRegularExpression regexPush{QStringLiteral(R"((\S+)\s+pushes to (\S+)\s+\(([^)]*)\))")};

    //    for (auto &line : lines) {
    //        line = line.trimmed();

    //        if (mode == GitPull) {
    //            const auto match = regexPull.match(line); // clazy:exclude=use-static-qregularexpression
    //            if (match.hasMatch()) {
    //                RemoteBranch branch;
    //                branch.configuredPull = true;
    //                branch.name = match.captured(1);
    //                branch.remotePullBranch = match.captured(2);
    //                branches.append(branch);
    //            }
    //        } else if (mode == GitPush) {
    //            const auto match = regexPush.match(line); // clazy:exclude=use-static-qregularexpression
    //            if (match.hasMatch()) {
    //                int index{-1};
    //                RemoteBranch branch;
    //                for (int i = 0; i < branches.size(); ++i) {
    //                    if (branches[i].name == match.captured(1)) {
    //                        index = i;
    //                        branch = branches[i];
    //                        break;
    //                    }
    //                }

    //                branch.configuredPush = true;
    //                branch.name = match.captured(1);
    //                if (match.captured(3) == QStringLiteral("fast-forwardable"))
    //                    branch.status = RemoteBranch::Status::FastForwardable;
    //                else if (match.captured(3) == QStringLiteral("up to date"))
    //                    branch.status = RemoteBranch::Status::UpToDate;
    //                else if (match.captured(3) == QStringLiteral("local out of date"))
    //                    branch.status = RemoteBranch::Status::LocalOutOfDate;
    //                else
    //                    qCDebug(KOMMITLIB_LOG) << "Unknown status" << match.captured(3);

    //                branch.remotePushBranch = match.captured(2);

    //                if (index == -1)
    //                    branches.append(branch);
    //                else
    //                    branches.replace(index, branch);
    //            }
    //        }
    //        if (line.startsWith(QStringLiteral("* remote"))) {
    //            // name = line.remove(QStringLiteral("* remote ")).trimmed();
    //        } else if (line.startsWith(QStringLiteral("HEAD branch: "))) {
    //            //            headBranch = line.remove(QStringLiteral("HEAD branch: ")).trimmed();
    //        } else if (line.startsWith(QStringLiteral("Push  URL:"))) {
    //            //            pushUrl = line.remove(QStringLiteral("Push  URL:")).trimmed();
    //        } else if (line.startsWith(QStringLiteral("Fetch URL:"))) {
    //            //            fetchUrl = line.remove(QStringLiteral("Fetch URL:")).trimmed();
    //        } else if (line == QStringLiteral("Local branches configured for 'git pull':")) {
    //            mode = GitPull;
    //            continue;
    //        } else if (line == QStringLiteral("Local refs configured for 'git push':")) {
    //            mode = GitPush;
    //            continue;
    //        }
    //    }
    //    qCDebug(KOMMITLIB_LOG) << branches.size();
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

QList<Branch *> Remote::branches() const
{
    return mBranches;
}

bool Remote::connected() const
{
    return mConnected;
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
