/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagsmodel.h"
#include "authorsmodel.h"
#include "gitmanager.h"
#include "gittag.h"

#include <KLocalizedString>

namespace Git
{

TagsModel::TagsModel(Manager *git, QObject *parent)
    : AbstractGitItemsModel(git, parent)
{
}

int TagsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int TagsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(TagsModelRoles::LastColumn) + 1;
}

QVariant TagsModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    auto tag = mData.at(index.row());

    switch (index.column()) {
    case Name:
        return tag->name();
    case Subject:
        return tag->message();
    case Tagger:
        return tag->taggerName() + QStringLiteral(" ") + tag->taggerEmail();
    case Committer:
        return tag->commiterName() + QStringLiteral(" ") + tag->commiterEmail();
    }
    return {};
}

QVariant TagsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section) {
        case Name:
            return i18n("Name");
        case Subject:
            return i18n("Subject");
        case Tagger:
            return i18n("Tagger");
        case Committer:
            return i18n("Committer");
        }

    return {};
}

Tag *TagsModel::fromIndex(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return nullptr;

    return mData.at(index.row());
}

void TagsModel::fill()
{
    // TODO: remove < and > from email address
    // TODO: ass objecttype
    qDeleteAll(mData);
    mData.clear();

    const auto tagsNames = mGit->readAllNonEmptyOutput({QStringLiteral("--no-pager"), QStringLiteral("tag"), QStringLiteral("--list")});

    for (const auto &tagName : tagsNames) {
        auto tag = new Tag;
        mData.append(tag);
        tag->setName(tagName);
        auto message = mGit->readAllNonEmptyOutput({QStringLiteral("tag"), QStringLiteral("-n99"), tagName, QStringLiteral("--format=%(subject)")}).join('\n');
        tag->setMessage(message);

        auto l = mGit->runGit({QStringLiteral("--no-pager"),
                               QStringLiteral("tag"),
                               QStringLiteral("-n99"),
                               tagName,
                               QStringLiteral("--format=%(taggername)\r%(taggeremail)\r%(committername)\r%(committeremail)\r%(tag)")});

        auto parts = l.split('\r');

        if (parts.size() < 4)
            continue;
        tag->setTaggerName(parts.at(0).trimmed());
        tag->setTaggerEmail(parts.at(1).trimmed());
        tag->setCommiterName(parts.at(2).trimmed());
        tag->setCommiterEmail(parts.at(3).trimmed());

        if (tag->taggerName().isEmpty())
            tag->setTaggerName(tag->commiterName());
        if (tag->taggerEmail().isEmpty())
            tag->setTaggerEmail(tag->commiterEmail());
        if (mGit->authorsModel()) {
            auto author = mGit->authorsModel()->findOrCreate(tag->taggerName(), tag->taggerEmail());
            author->tags++;
        }
    }
}

} // namespace Git
