/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gitloglist.h"
#include "caches/branchescache.h"
#include "entities/commit.h"
#include "gitmanager.h"

#include <QDateTime>
#include <utility>

namespace Git
{

void readLine(const QString &line, const QString &seprator, QList<QString *> list)
{
    const auto parts = line.split(seprator);
    if (parts.size() != list.size())
        return;

    for (auto i = 0; i < parts.size(); i++)
        *list[i] = parts.at(i);
}

QString LogList::branchName(const QString &refLog)
{
    if (refLog.isEmpty())
        return {};

    //    auto parts = refLog.split(",");
    //    if (parts.size() < 2)
    //        return QString();

    //    return parts.at(1).mid(1, parts.at(1).indexOf(")") - 1);
    for (auto &b : mBranches)
        if (refLog.contains(b))
            return b;
    return {};
}

void LogList::initChilds()
{
    for (auto i = rbegin(); i != rend(); i++) {
        auto &log = *i;
        // for (auto &p : log->parents())
        // mDataByCommitHashLong.value(p)->mChildren.append(log->commitHash());
    }
}

const QString &LogList::branch() const
{
    return mBranch;
}

void LogList::setBranch(const QString &newBranch)
{
    mBranch = newBranch;
}

Commit *LogList::findByHash(const QString &hash, int *index) const
{
    for (int i = 0; i < size(); ++i) {
        if (at(i)->commitHash() == hash) {
            if (index)
                *index = i;
            return at(i);
        }
    }
    if (index)
        *index = -1;
    return nullptr;
}

LogList::LogList()
    : QList<Commit *>()
{
}

LogList::LogList(QString branch)
    : QList<Commit *>()
    , mBranch(std::move(branch))
{
}

void LogList::load(Git::Manager *git)
{
    /*
% -- raw %                    N -- notes                    d -- ref name in brackets     n -- newline
B -- raw body                 P -- parent hash              e -- encoding                 p -- abbreviated parent hash
C -- color                    T -- tree hash                f -- sanitized subject        s -- subject
D -- ref name                 a -- author details           g -- reflog                   t -- abbreviated tree hash
G -- GPG details              b -- body                     h -- abbreviated commit hash  w -- switch line wrapping
H -- commit hash              c -- committer details        m -- mark                     x -- hex code
*/
    qDeleteAll(*this);
    clear();
    mDataByCommitHashLong.clear();

    mBranches = git->branches()->names(Git::BranchType::LocalBranch);

    QStringList args{QStringLiteral("--no-pager"),
                     QStringLiteral("log"),
                     QStringLiteral("--topo-order"),
                     QStringLiteral("--no-color"),
                     QStringLiteral("--parents"),
                     QStringLiteral("--boundary"),
                     QStringLiteral("--pretty=format:'SEP%m%HX%hX%P%n"
                                    "%cnX%ceX%cI%n"
                                    "%anX%aeX%aI%n"
                                    "%d%n"
                                    "%at%n"
                                    "%s%n"
                                    "%b%n'")};

    if (!mBranch.isEmpty())
        args.insert(2, mBranch);

    // TODO: remove this singelton call
    const auto ret = QString(Manager::instance()->runGit(args));
    if (ret.startsWith(QStringLiteral("fatal:")))
        return;

    const auto parts = ret.split(QStringLiteral("SEP>"));

    //    for (auto &p : parts) {
    //        auto lines = p.split(QLatin1Char('\n'));
    //        if (lines.size() < 4)
    //            continue;

    //        auto d = new Log;
    //        QString commitDate;
    //        QString authDate;
    //        QString parentHash;
    //        readLine(lines.at(0), QStringLiteral("X"), {&d->mCommitHash, &d->mCommitShortHash, &parentHash});
    //        readLine(lines.at(1), QStringLiteral("X"), {&d->mCommitterName, &d->mCommitterEmail, &commitDate});
    //        readLine(lines.at(2), QStringLiteral("X"), {&d->mAuthorName, &d->mAuthorEmail, &authDate});

    //        if (!parentHash.isEmpty())
    //            d->mParentHash = parentHash.split(QStringLiteral(" "));
    //        d->mRefLog = lines.at(3);
    //        d->mSubject = lines.at(5);
    //        d->mCommitDate = QDateTime::fromString(commitDate, Qt::ISODate);
    //        d->mAuthDate = QDateTime::fromString(authDate, Qt::ISODate);
    //        d->mBody = lines.mid(5).join(QStringLiteral("\n"));
    //        append(d);
    //        mDataByCommitHashLong.insert(d->commitHash(), d);
    //        mDataByCommitHashLong.insert(d->commitShortHash(), d);
    //    }
    //    std::sort(begin(), end(), [](GitLog *log1,GitLog *log2){
    //        return log1->commitDate() < log2->commitDate();
    //    });
    initChilds();
    initGraph();
}

void LogList::initGraph()
{
    // LanesFactory factory;
    // for (auto i = rbegin(); i != rend(); i++) {
    //     auto &log = *i;
    //     log->mLanes = factory.apply(log);
    // }
}

}
