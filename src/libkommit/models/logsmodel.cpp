/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "logsmodel.h"
#include "entities/commit.h"
#include "gitmanager.h"
#include "qdebug.h"

#include <KLocalizedString>
#include <QDebug>

#include <git2/commit.h>

namespace Git
{

namespace Impl
{

void readLine(const QString &line, const QString &separator, QList<QString *> list)
{
    const auto parts = line.split(separator);
    if (parts.size() != list.size())
        return;

    for (auto i = 0; i < parts.size(); ++i)
        *list[i] = parts.at(i);
}
struct LanesFactory {
    QStringList _hashes;

    QList<int> findByChild(const QString &hash)
    {
        int index{0};
        QList<int> ret;
        for (auto const &h : std::as_const(_hashes)) {
            if (hash == h)
                ret.append(index);
            index++;
        }
        return ret;
    }

    int indexOfChild(const QString &hash)
    {
        int index{0};
        for (auto const &h : std::as_const(_hashes)) {
            if (hash == h)
                return index;
            index++;
        }
        return -1;
    }

    QVector<GraphLane> initLanes(const QString &myHash, int &myIndex)
    {
        if (_hashes.empty())
            return {};

        while (!_hashes.empty() && _hashes.last() == QString())
            _hashes.removeLast();

        int index{0};
        QVector<GraphLane> lanes;
        lanes.reserve(_hashes.size());
        for (const auto &hash : std::as_const(_hashes)) {
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
                _hashes.append(h);
                index = _hashes.size() - 1;
            } else {
                _hashes.replace(index, h);
            }
            ret.append(index);
        }
        return ret;
    }

    void start(const QString &hash, QVector<GraphLane> &lanes)
    {
        Q_UNUSED(hash)
        _hashes.append(QString());
        set(_hashes.size() - 1, GraphLane::Start, lanes);
    }

    void join(const QString &hash, QVector<GraphLane> &lanes, int &myIndex)
    {
        int firstIndex{-1};
        const auto list = findByChild(hash);

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
            _hashes.replace(*i, QString());
        }
        myIndex = firstIndex;
    }

    void fork(const QStringList &childrenList, QVector<GraphLane> &lanes, int myInedx)
    {
        auto list = setHashes(childrenList, -1);
        auto children = childrenList;
        lanes.reserve(_hashes.size());

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
            _hashes.replace(i, children.takeFirst());
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
            myIndex = _hashes.size() - 1;
        }

        if (!log->children().empty()) {
            fork(log->children(), lanes, myIndex);
        } else if (myIndex != -1) {
            lanes[myIndex].mType = GraphLane::End;
        }

        return lanes;
    }
};

} // namespace Impl

LogsModel::LogsModel(Manager *git, QObject *parent)
    : AbstractGitItemsModel(git, parent)
{
}

LogsModel::~LogsModel()
{
    qDeleteAll(mData);
    mData.clear();
}

const QString &LogsModel::branch() const
{
    return mBranch;
}

void LogsModel::setBranch(const QString &newBranch)
{
    mBranch = newBranch;

    beginResetModel();
    fill();
    endResetModel();
}

int LogsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int LogsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mFullDetails ? 3 : 1;
}

QVariant LogsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Vertical)
        return section + 1;

    if (mFullDetails) {
        switch (section) {
        case 0:
            return i18n("Message");
        case 1:
            return i18n("Date");
        case 2:
            return i18n("Author");
        }
    } else {
        switch (section) {
        case 0:
            return i18n("Graph");
        case 1:
            return i18n("Message");
        }
    }
    return {};
}

Commit *LogsModel::at(int index) const
{
    if (index < 0 || index >= mData.size())
        return nullptr;

    return mData.at(index);
}

QVariant LogsModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return {};
    auto log = fromIndex(index);
    if (!log)
        return {};

    if (mFullDetails) {
        switch (index.column()) {
        case 0:
            return log->subject();
        case 1: {
            if (mCalendar.isValid())
                return log->committer()->time().toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"), mCalendar);

            return log->committer()->time();
        }
        case 2:
            return log->author()->name();
        }
    } else {
        switch (index.column()) {
        case 0:
            return QString();
        case 1:
            return log->subject();
        }
    }

    return {};
}

Commit *LogsModel::fromIndex(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return nullptr;

    return mData.at(index.row());
}

QModelIndex LogsModel::findIndexByHash(const QString &hash) const
{
    int idx{0};
    for (auto &log : mData)
        if (log->commitHash() == hash)
            return index(idx);
        else
            idx++;
    return {};
}

Commit *LogsModel::findLogByHash(const QString &hash, LogMatchType matchType) const
{
    QList<Commit *>::ConstIterator i;

    switch (matchType) {
    case LogMatchType::ExactMatch:
        i = std::find_if(mData.begin(), mData.end(), [&hash](Commit *log) {
            return log->commitHash() == hash;
        });
        break;
    case LogMatchType::BeginMatch:
        i = std::find_if(mData.begin(), mData.end(), [&hash](Commit *log) {
            return log->commitHash().startsWith(hash);
        });
        break;
    }
    if (i == mData.end())
        return nullptr;
    return *i;
}

void LogsModel::fill()
{
    constexpr int GIT_SUCCESS{0};
    qDeleteAll(mData);
    mData.clear();
    mDataByCommitHashLong.clear();
    mDataByCommitHashShort.clear();
    mSeenHases.clear();

    git_revwalk *walker;
    git_oid oid;

    git_revwalk_new(&walker, mGit->mRepo);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);

    if (mBranch.isEmpty()) {
        // git_revwalk_push_head(walker);

        git_branch_iterator *it;
        git_branch_iterator_new(&it, mGit->mRepo, GIT_BRANCH_ALL);

        git_reference *ref;
        git_branch_t b;

        QStringList list;
        while (!git_branch_next(&ref, &b, it)) {
            //        if (!git_reference_is_branch(ref))
            //            continue;
            auto refname = git_reference_name(ref);

            git_revwalk_push_ref(walker, refname);
        }
        git_branch_iterator_free(it);

    } else {
        git_reference *ref;
        auto n = git_branch_lookup(&ref, mGit->mRepo, mBranch.toLocal8Bit().data(), GIT_BRANCH_ALL);

        if (n)
            return;

        auto refName = git_reference_name(ref);
        git_revwalk_push_ref(walker, refName);

        git_reference_free(ref);
    }

    while (git_revwalk_next(&oid, walker) == GIT_SUCCESS) {
        git_commit *commit;

        if (git_commit_lookup(&commit, mGit->mRepo, &oid)) {
            fprintf(stderr, "Failed to lookup the next object\n");
            return;
        }

        auto d = new Commit{commit};

        mData.append(d);
        mDataByCommitHashLong.insert(d->commitHash(), d);
        mDataByCommitHashShort.insert(d->commitShortHash(), d);
    }

    git_revwalk_free(walker);

    struct wrapper {
        QList<Commit *> mData;
        QMap<QString, Commit *> mDataByCommitHashLong;
        QMap<QString, Commit *> mDataByCommitHashShort;
        git_repository *repo;
    };
    auto cb = [](git_reference *reference, void *payload) -> int {
        if (git_reference_is_note(reference))
            return 0;

        auto w = reinterpret_cast<wrapper *>(payload);

        auto target = git_reference_target(reference);
        if (!target) {
            qDebug() << "No target" << git_reference_is_branch(reference) << git_reference_is_note(reference) << git_reference_is_remote(reference)
                     << git_reference_is_tag(reference);

            git_reference_free(reference);
            return 0;
        }

        QString hash = git_oid_tostr_s(target);
        auto c = w->mDataByCommitHashLong.value(hash);
        if (c) {
            c->mReference.reset(new Reference{reference});
        }

        return 0;
    };
    wrapper w;
    w.mData = mData;
    w.mDataByCommitHashLong = mDataByCommitHashLong;
    w.mDataByCommitHashShort = mDataByCommitHashShort;
    w.repo = mGit->mRepo;
    git_reference_foreach(mGit->mRepo, cb, &w);

    initChilds();
    initGraph();
}

bool LogsModel::fullDetails() const
{
    return mFullDetails;
}

void LogsModel::setFullDetails(bool newFullDetails)
{
    beginResetModel();
    mFullDetails = newFullDetails;
    endResetModel();
}

void LogsModel::initChilds()
{
    for (auto i = mData.rbegin(); i != mData.rend(); i++) {
        auto &log = *i;
        for (auto &p : log->parents())
            mDataByCommitHashLong.value(p)->mChildren.append(log->commitHash());
    }
}

void LogsModel::initGraph()
{
    Impl::LanesFactory factory;
    for (auto i = mData.rbegin(); i != mData.rend(); i++) {
        auto &log = *i;
        log->mLanes = factory.apply(log);
    }
}

QString LogsModel::calendarType() const
{
    return mCalendar.name();
}

void LogsModel::setCalendarType(const QString &newCalendarType)
{
    if (mCalendar.name() != newCalendarType) {
        beginResetModel();
        mCalendar = QCalendar(newCalendarType);
        endResetModel();
    }
}
}

#include "moc_logsmodel.cpp"
