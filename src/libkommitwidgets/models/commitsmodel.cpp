/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitsmodel.h"
#include "caches/commitscache.h"
#include "entities/commit.h"
#include "gitmanager.h"

#include <KLocalizedString>
#include <QDebug>

#include <git2/commit.h>

struct CommitsLaneData {
    QSharedPointer<Git::Commit> commit;
    QVector<GraphLane> lanes;
};

namespace Impl
{

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
        // TODO: fix me
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
        // TODO: fix me
        const auto list = setHashes(childrenList, -1);
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
        for (int i : list) {
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
    QVector<GraphLane> apply(Git::Commit *log)
    {
        int myIndex = -1;
        QVector<GraphLane> lanes = initLanes(log->commitHash(), myIndex);
        // TODO: fix me
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

class CommitsModelPrivate
{
    CommitsModel *q_ptr;
    Q_DECLARE_PUBLIC(CommitsModel);

public:
    CommitsModelPrivate(CommitsModel *parent);

    void initChilds();
    void initGraph();

    bool fullDetails{false};
    QSharedPointer<Git::Branch> branch;
    QList<CommitsLaneData *> data;
    QList<QSharedPointer<Git::Commit>> list;
    QStringList branches;
    QMap<QString, QSharedPointer<Git::Commit>> dataByCommitHashLong;
    QMap<QString, QSharedPointer<Git::Commit>> dataByCommitHashShort;
    QCalendar calendar;
    QSet<QString> seenHashes;
};

CommitsModel::CommitsModel(Git::Manager *git, QObject *parent)
    : AbstractGitItemsModel(git, parent)
    , d_ptr{new CommitsModelPrivate{this}}
{
    connect(git->commits(), &Git::CommitsCache::added, this, &CommitsModel::reload);
    connect(git, &Git::Manager::pathChanged, this, &CommitsModel::reload);
}

CommitsModel::~CommitsModel()
{
    Q_D(CommitsModel);
    delete d;
}

QSharedPointer<Git::Branch> CommitsModel::branch() const
{
    Q_D(const CommitsModel);
    return d->branch;
}

void CommitsModel::setBranch(QSharedPointer<Git::Branch> newBranch)
{
    Q_D(CommitsModel);

    if (d->branch == newBranch)
        return;

    d->branch = newBranch;

    beginResetModel();
    reload();
    endResetModel();
}

int CommitsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    Q_D(const CommitsModel);

    return d->list.size();
}

int CommitsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    Q_D(const CommitsModel);

    return d->fullDetails ? 3 : 1;
}

QVariant CommitsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const CommitsModel);

    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Vertical)
        return section + 1;

    if (d->fullDetails) {
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

QSharedPointer<Git::Commit> CommitsModel::at(int index) const
{
    Q_D(const CommitsModel);

    if (index < 0 || index >= d->list.size())
        return nullptr;

    return d->list.at(index);
}

QVariant CommitsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const CommitsModel);

    if (role != Qt::DisplayRole)
        return {};
    auto log = fromIndex(index);
    if (!log)
        return {};

    if (d->fullDetails) {
        switch (index.column()) {
        case 0:
            return log->summary();
        case 1: {
            if (d->calendar.isValid())
                return log->committer()->time().toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"), d->calendar);

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
            return log->summary();
        }
    }

    return {};
}

QSharedPointer<Git::Commit> CommitsModel::fromIndex(const QModelIndex &index) const
{
    Q_D(const CommitsModel);

    if (!index.isValid() || index.row() < 0 || index.row() >= d->list.size())
        return nullptr;

    return d->list.at(index.row());
}

QVector<GraphLane> CommitsModel::lanesFromIndex(const QModelIndex &index) const
{
    Q_D(const CommitsModel);

    if (!index.isValid() || index.row() < 0 || index.row() >= d->data.size())
        return {};

    return d->data.at(index.row())->lanes;
}

QModelIndex CommitsModel::findIndexByHash(const QString &hash) const
{
    Q_D(const CommitsModel);

    int idx{0};
    for (auto &log : d->list)
        if (log->commitHash() == hash)
            return index(idx);
        else
            idx++;
    return {};
}

QSharedPointer<Git::Commit> CommitsModel::findLogByHash(const QString &hash, LogMatchType matchType) const
{
    Q_D(const CommitsModel);

    QList<QSharedPointer<Git::Commit>>::ConstIterator i;

    switch (matchType) {
    case LogMatchType::ExactMatch:
        i = std::find_if(d->list.begin(), d->list.end(), [&hash](QSharedPointer<Git::Commit> log) {
            return log->commitHash() == hash;
        });
        break;
    case LogMatchType::BeginMatch:
        i = std::find_if(d->list.begin(), d->list.end(), [&hash](QSharedPointer<Git::Commit> log) {
            return log->commitHash().startsWith(hash);
        });
        break;
    }
    if (i == d->list.end())
        return nullptr;
    return *i;
}

void CommitsModel::reload()
{
    Q_D(CommitsModel);

    qDeleteAll(d->data);
    d->data.clear();

    if (mGit->isValid()) {
        if (d->branch.isNull())
            d->list = mGit->commits()->allCommits();
        else
            d->list = mGit->commits()->commitsInBranch(d->branch);
        d->dataByCommitHashLong.clear();

        d->data.reserve(d->list.size());
        for (auto const &commit : std::as_const(d->list)) {
            d->dataByCommitHashLong.insert(commit->commitHash(), commit);
            d->data << new CommitsLaneData{commit, {}};
        }
    } else {
        d->list.clear();
        d->dataByCommitHashLong.clear();
    }
    d->initChilds();
    d->initGraph();
}

bool CommitsModel::fullDetails() const
{
    Q_D(const CommitsModel);

    return d->fullDetails;
}

void CommitsModel::setFullDetails(bool newFullDetails)
{
    Q_D(CommitsModel);

    if (d->fullDetails == newFullDetails)
        return;

    beginResetModel();
    d->fullDetails = newFullDetails;
    endResetModel();
}

void CommitsModelPrivate::initChilds()
{
    // for (auto &d : data) {
    // auto &log = *i;
    // TODO: check this
    // for (auto &p : d.commit->parents())
    // mDataByCommitHashLong.value(p)->mChildren.append(log->commitHash());
    // }
}

void CommitsModelPrivate::initGraph()
{
    Impl::LanesFactory factory;
    for (auto i = data.rbegin(); i != data.rend(); i++) {
        auto d = *i;
        d->lanes = factory.apply(d->commit.data());
    }
}

QString CommitsModel::calendarType() const
{
    Q_D(const CommitsModel);

    return d->calendar.name();
}

void CommitsModel::setCalendarType(const QString &newCalendarType)
{
    Q_D(CommitsModel);

    if (d->calendar.name() != newCalendarType) {
        beginResetModel();
        d->calendar = QCalendar(newCalendarType);
        endResetModel();
    }
}

void CommitsModel::clear()
{
    Q_D(CommitsModel);

    beginResetModel();
    d->list.clear();
    endResetModel();
}

CommitsModelPrivate::CommitsModelPrivate(CommitsModel *parent)
    : q_ptr(parent)
{
}

#include "moc_commitsmodel.cpp"
