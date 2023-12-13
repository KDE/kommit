/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gitloglist.h"
#include "entities/commit.h"
#include "gitgraphlane.h"
#include "gitmanager.h"

#include <QDateTime>
#include <utility>

namespace Git
{

struct LanesFactory {
    QStringList mHashes;

    QList<int> findByChild(const QString &hash)
    {
        int index{0};
        QList<int> ret;
        for (auto const &h : std::as_const(mHashes)) {
            if (hash == h)
                ret.append(index);
            index++;
        }
        return ret;
    }

    int indexOfChild(const QString &hash)
    {
        int index{0};
        for (auto const &h : std::as_const(mHashes)) {
            if (hash == h)
                return index;
            index++;
        }
        return -1;
    }

    QVector<GraphLane> initLanes(const QString &myHash, int &myIndex)
    {
        if (mHashes.empty())
            return {};

        while (!mHashes.empty() && mHashes.last() == QString())
            mHashes.removeLast();

        int index{0};
        QVector<GraphLane> lanes;
        lanes.reserve(mHashes.size());
        for (const auto &hash : std::as_const(mHashes)) {
            if (hash == QString()) {
                lanes.append(GraphLane::Transparent);
            } else {
                if (hash == myHash) {
                    lanes.append(GraphLane::Node);
                    myIndex = index;
                } else {
                    lanes.append(hash == myHash ? GraphLane::Node : GraphLane::Pipe);
                }
            }
            index++;
        }
        return lanes;
    }

    QList<int> setHashes(const QStringList &children, int myIndex)
    {
        QList<int> ret;
        bool myIndexSet{myIndex == -1};
        int index;

        for (const auto &h : children) {
            index = -1;
            if (!myIndexSet) {
                index = myIndex;
                myIndexSet = true;
            }
            if (index == -1)
                index = indexOfChild(QString());

            if (index == -1) {
                mHashes.append(h);
                index = mHashes.size() - 1;
            } else {
                mHashes.replace(index, h);
            }
            ret.append(index);
        }
        return ret;
    }

    void start(const QString &hash, QVector<GraphLane> &lanes)
    {
        Q_UNUSED(hash)
        mHashes.append(QString());
        set(mHashes.size() - 1, GraphLane::Start, lanes);
    }

    void join(const QString &hash, QVector<GraphLane> &lanes, int &myIndex)
    {
        int firstIndex{-1};
        auto list = findByChild(hash);

        for (auto i = list.begin(); i != list.end(); ++i) {
            if (firstIndex == -1) {
                firstIndex = *i;
                set(*i, list.contains(myIndex) ? GraphLane::Node : GraphLane::End, lanes);
            } else {
                auto lane = lanes.at(*i);
                lane.mBottomJoins.append(firstIndex);
                lane.mType = GraphLane::Transparent;
                set(*i, lane, lanes);
            }
            mHashes.replace(*i, QString());
        }
        myIndex = firstIndex;
    }

    void fork(const QStringList &childrenList, QVector<GraphLane> &lanes, int myInedx)
    {
        auto list = setHashes(childrenList, -1);
        auto children = childrenList;
        lanes.reserve(mHashes.size());

        if (myInedx != -1 && lanes.size() <= myInedx)
            lanes.resize(myInedx + 1);

        if (myInedx != -1 && childrenList.size() == 1) {
            auto &l = lanes[list.first()];

            if (list.first() == myInedx) {
                if (l.type() == GraphLane::None)
                    l.mType = GraphLane::Transparent;
                if (l.type() == GraphLane::End)
                    l.mType = GraphLane::Node;
            } else {
                l.mUpJoins.append(myInedx);
                lanes[myInedx].mType = GraphLane::End;
            }

            return;
        }
        for (int &i : list) {
            if (lanes.size() <= i)
                lanes.resize(i + 1);

            auto &l = lanes[i];
            if (i == myInedx) {
                l.mType = GraphLane::Node;
            } else {
                if (l.type() == GraphLane::None)
                    l.mType = GraphLane::Transparent;
                if (l.type() == GraphLane::End)
                    l.mType = GraphLane::Node;

                l.mUpJoins.append(myInedx);
            }
            mHashes.replace(i, children.takeFirst());
        }
    }

    void set(int index, const GraphLane &lane, QVector<GraphLane> &lanes)
    {
        if (index < lanes.size())
            lanes.replace(index, lane);
        else
            lanes.append(lane);
    }
    QVector<GraphLane> apply(Commit *log)
    {
        int myIndex = -1;
        QVector<GraphLane> lanes = initLanes(log->commitHash(), myIndex);

        if (!log->parents().empty())
            join(log->commitHash(), lanes, myIndex);
        else if (!log->children().empty()) {
            start(log->children().first(), lanes);
            myIndex = mHashes.size() - 1;
        }

        if (!log->children().empty()) {
            fork(log->children(), lanes, myIndex);
        } else if (myIndex != -1) {
            lanes[myIndex].mType = GraphLane::End;
        }

        return lanes;
    }
};

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
        for (auto &p : log->parents())
            mDataByCommitHashLong.value(p)->mChildren.append(log->commitHash());
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

    mBranches = git->branchesNames(Git::Manager::BranchType::LocalBranch);

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
    LanesFactory factory;
    for (auto i = rbegin(); i != rend(); i++) {
        auto &log = *i;
        log->mLanes = factory.apply(log);
    }
}

}
