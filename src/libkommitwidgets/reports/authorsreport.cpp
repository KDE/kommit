/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "authorsreport.h"

#include <entities/commit.h>
#include <entities/signature.h>
#include <entities/tag.h>
#include <gitmanager.h>

#include <KLocalizedString>

AuthorsReport::AuthorsReport(Git::Manager *git, QObject *parent)
    : AbstractReport{git, parent}
{
}

void AuthorsReport::reload()
{
    beginResetModel();
    qDeleteAll(mData);
    mData.clear();

    auto commitCb = [this](QSharedPointer<Git::Commit> commit) {
        findOrCreate(commit->author(), AuthorCreateReason::AuthoredCommit);
        findOrCreate(commit->committer(), AuthorCreateReason::Commit);
    };
    auto tagCb = [this](QSharedPointer<Git::Tag> tag) {
        findOrCreate(tag->tagger(), AuthorCreateReason::Tag);
    };

    mGit->forEachCommits(commitCb, "");
    mGit->forEachTags(tagCb);
    endResetModel();
}

QString AuthorsReport::name() const
{
    return i18n("Commits count by author");
}

int AuthorsReport::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int AuthorsReport::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(AuthorsReportRoles::LastColumn);
}

QVariant AuthorsReport::headerData(int section, Qt::Orientation orientation, int role) const
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
    case Autheds:
        return i18n("Autheds");
    case Tags:
        return i18n("Tags");
    }
    return {};
}

QVariant AuthorsReport::data(const QModelIndex &index, int role) const
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
    case Autheds:
        return mData.at(index.row())->authoredCommits.count;
    case Tags:
        return mData.at(index.row())->tags.count;
    }

    return {};
}

void AuthorsReport::findOrCreate(QSharedPointer<Git::Signature> signature, AuthorCreateReason reason)
{
    auto authorIterator = std::find_if(mData.begin(), mData.end(), [&signature](Author *a) {
        return a->email == signature->email() && a->name == signature->name();
    });

    if (authorIterator != mData.end()) {
        switch (reason) {
        case Commit:
            (*authorIterator)->commits.increase(signature->time());
            break;
        case AuthoredCommit:
            (*authorIterator)->authoredCommits.increase(signature->time());
            break;
        case Tag:
            (*authorIterator)->tags.increase(signature->time());
            break;
        }
        return;
    }

    auto author = new Author;
    author->name = signature->name();
    author->email = signature->email();

    switch (reason) {
    case Commit:
        author->commits.begin(signature->time());
        break;
    case AuthoredCommit:
        author->authoredCommits.begin(signature->time());
        break;
    case Tag:
        author->tags.begin(signature->time());
        break;
    }

    authorIterator = std::upper_bound(mData.begin(), mData.end(), author, [](Author *author, const Author *a) {
        auto c = QString::compare(author->name, a->name, Qt::CaseInsensitive);
        if (!c)
            return QString::compare(author->email, a->email, Qt::CaseInsensitive) < 0;
        return c < 0;
    });

    mData.insert(authorIterator, author);
}

void AuthorsReport::DatesRange::begin(const QDateTime &time)
{
    count = 1;
    first = time;
}

void AuthorsReport::DatesRange::increase(const QDateTime &time)
{
    last = time;
    count++;
}

// #include "authorsreport.moc"
