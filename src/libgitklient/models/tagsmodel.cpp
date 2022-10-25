/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagsmodel.h"
#include "gitmanager.h"
#include "gittag.h"

#include "libgitklient_debug.h"

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
    return 4;
}

QVariant TagsModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    auto tag = mData.at(index.row());

    switch (index.column()) {
    case 0:
        return tag->name();
    case 1:
        return tag->message();
    case 2:
        return tag->taggerName() + QStringLiteral(" ") + tag->taggerEmail();
    case 3:
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
        case 0:
            return i18n("Name");
        case 1:
            return i18n("Subject");
        case 2:
            return i18n("Tagger");
        case 3:
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
    //TODO: remove < and > from email address
    //TODO: ass objecttype
    qDeleteAll(mData);
    mData.clear();

    const auto tagsNames = mGit->readAllNonEmptyOutput({QStringLiteral("--no-pager"), QStringLiteral("tag"), QStringLiteral("--list")});

    for (const auto &tagName: tagsNames) {
        auto tag = new Tag;
        mData.append(tag);
        tag->setName(tagName);
        auto message = mGit->readAllNonEmptyOutput({QStringLiteral("tag"), QStringLiteral("-n99"), tagName, QStringLiteral("--format=%(subject)")}).join('\n');
        qDebug() << "message for" << tagName << "is" << message;
        tag->setMessage(message);

        auto l = mGit->runGit({QStringLiteral("--no-pager"),
                               QStringLiteral("tag"),
                               QStringLiteral("-n99"),
                               tagName,
                               QStringLiteral("--format=%(taggername)\r%(taggeremail)\r%(committername)\r%(committeremail)\r%(tag)")});

        auto parts=l.split('\r');

        qDebug() << tagName << parts;
        if (parts.size() < 4)
            continue;
        tag->setTaggerName(parts.at(0).trimmed());
        tag->setTaggerEmail(parts.at(1).trimmed());
        tag->setCommiterName(parts.at(2).trimmed());
        tag->setCommiterEmail(parts.at(3).trimmed());
    }
}

} // namespace Git
