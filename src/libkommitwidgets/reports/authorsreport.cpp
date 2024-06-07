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
    setValueColumn(2);
}

AuthorsReport::~AuthorsReport()
{
    qDeleteAll(mData);
}

void AuthorsReport::reload()
{
    qDeleteAll(mData);
    mData.clear();

    auto max{0};

    auto commitCb = [this, &max](QSharedPointer<Git::Commit> commit) {
        findOrCreate(commit->author(), AuthorCreateReason::AuthoredCommit);
        auto a = findOrCreate(commit->committer(), AuthorCreateReason::Commit);

        max = qMax(max, a->commits.count);
    };
    auto tagCb = [this](QSharedPointer<Git::Tag> tag) {
        findOrCreate(tag->tagger(), AuthorCreateReason::Tag);
    };

    mGit->forEachCommits(commitCb, {});
    mGit->forEachTags(tagCb);

    clear();
    for (auto const &data : std::as_const(mData)) {
        addData({data->name, data->email, data->commits.count, data->authoredCommits.count, data->tags.count});
        extendRange(data->commits.count);
    }

    Q_EMIT reloaded();
}

QString AuthorsReport::name() const
{
    return i18n("Commits count by author");
}

int AuthorsReport::columnCount() const
{
    return 5;
}

QStringList AuthorsReport::headerData() const
{
    return {i18n("Name"), i18n("Email"), i18n("Commits"), i18n("Autheds"), i18n("Tags")};
}

bool AuthorsReport::supportChart() const
{
    return true;
}

QString AuthorsReport::axisXTitle() const
{
    return i18n("Author");
}

QString AuthorsReport::axisYTitle() const
{
    return i18n("Commits");
}

int AuthorsReport::labelsAngle() const
{
    return 90;
}

AuthorsReport::Author *AuthorsReport::findOrCreate(QSharedPointer<Git::Signature> signature, AuthorCreateReason reason)
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
        return *authorIterator;
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
    return author;
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

#include "moc_authorsreport.cpp"
