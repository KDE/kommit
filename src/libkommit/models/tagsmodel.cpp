/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagsmodel.h"
#include "authorsmodel.h"
#include "entities/tag.h"
#include "gitmanager.h"

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

    switch (static_cast<TagsModelRoles>(index.column())) {
    case TagsModelRoles::Name:
        return tag->name();
    case TagsModelRoles::Subject:
        return tag->message();
    case TagsModelRoles::Tagger:
        return QStringLiteral("%1 <%2>").arg(tag->tagger()->name(), tag->tagger()->email());
    case TagsModelRoles::Time:
        return tag->tagger()->time();
    }
    return {};
}

QVariant TagsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (static_cast<TagsModelRoles>(section)) {
        case TagsModelRoles::Name:
            return i18n("Name");
        case TagsModelRoles::Subject:
            return i18n("Subject");
        case TagsModelRoles::Tagger:
            return i18n("Tagger");
        case TagsModelRoles::Time:
            return i18n("Create time");
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
    qDeleteAll(mData);
    mData.clear();

    mGit->forEachTags([this](Tag *tag) {
        mData.append(tag);
        if (mGit->authorsModel())
            mGit->authorsModel()->findOrCreate(tag->tagger()->name(), tag->tagger()->email(), QDateTime(), AuthorsModel::Tag);
    });
}

} // namespace Git

#include "moc_tagsmodel.cpp"
