/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "authorsmodel.h"
#include <KLocalizedString>

namespace Git
{

AuthorsModel::AuthorsModel(Manager *git, QObject *parent)
    : AbstractGitItemsModel{git, parent}
{
}

int AuthorsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int AuthorsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(AuthorsModelsRoles::LastColumn);
}

QVariant AuthorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case Name:
        return i18n("Name");
    case Email:
        return i18n("Email");
    case Commits:
        return i18n("Commits");
    }
    return {};
}

QVariant AuthorsModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    switch (index.column()) {
    case Name:
        return mData.at(index.row())->name;
    case Email:
        return mData.at(index.row())->email;
    case Commits:
        return mData.at(index.row())->commits.count;
    }

    return {};
}

Author *AuthorsModel::findOrCreate(const QString &name, const QString &email)
{
    QMutexLocker locker(&mDataMutex);

    auto authorIterator = std::find_if(mData.begin(), mData.end(), [&name, &email](Author *a) {
        return a->email == email && a->name == name;
    });

    if (authorIterator != mData.end())
        return *authorIterator;

    auto author = new Author;
    author->name = name;
    author->email = email;

    authorIterator = std::upper_bound(mData.begin(), mData.end(), qMakePair(name, email), [](const QPair<QString, QString> &data, const Author *a) {
        auto c = QString::compare(data.first, a->name, Qt::CaseInsensitive);
        if (!c)
            return QString::compare(data.second, a->email, Qt::CaseInsensitive) < 0;
        return c < 0;
    });

    const int idx = authorIterator - mData.begin();
    beginInsertRows(QModelIndex(), idx, idx);
    mData.insert(authorIterator, author);
    endInsertRows();
    return author;
}

Author *AuthorsModel::findOrCreate(const QString &name, const QString &email, const QDateTime &time, AuthorCreateReason reason)
{
    QMutexLocker locker(&mDataMutex);

    auto authorIterator = std::find_if(mData.begin(), mData.end(), [&name, &email](Author *a) {
        return a->email == email && a->name == name;
    });

    if (authorIterator != mData.end()) {
        switch (reason) {
        case Commit:
            (*authorIterator)->commits.increase(time);
            break;
        case AuthoredCommit:
            (*authorIterator)->authoredCommits.increase(time);
            break;
        case Tag:
            (*authorIterator)->tags.increase(time);
            break;
        }
        return *authorIterator;
    }

    auto author = new Author;
    author->name = name;
    author->email = email;

    switch (reason) {
    case Commit:
        author->commits.begin(time);
        break;
    case AuthoredCommit:
        author->authoredCommits.begin(time);
        break;
    case Tag:
        author->tags.begin(time);
        break;
    }

    authorIterator = std::upper_bound(mData.begin(), mData.end(), author, [](Author *author, const Author *a) {
        auto c = QString::compare(author->name, a->name, Qt::CaseInsensitive);
        if (!c)
            return QString::compare(author->email, a->email, Qt::CaseInsensitive) < 0;
        return c < 0;
    });

    int idx = authorIterator - mData.begin();
    beginInsertRows(QModelIndex(), idx, idx);
    mData.insert(authorIterator, author);
    endInsertRows();
    return author;
}

void AuthorsModel::clear()
{
    beginResetModel();
    qDeleteAll(mData);
    mData.clear();
    endResetModel();
}

void AuthorsModel::fill()
{
}

void DatesRange::begin(const QDateTime &time)
{
    count = 1;
    first = time;
}

void DatesRange::increase(const QDateTime &time)
{
    last = time;
    count++;
}

} // namespace Git
