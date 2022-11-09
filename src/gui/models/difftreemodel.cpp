/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "difftreemodel.h"

#include <GitKlientSettings.h>

DiffTreeModel::DiffTreeModel(QObject *parent)
    : TreeModel(parent)
{
    setLastPartAsData(true);
}

void DiffTreeModel::addFile(const FileStatus &file)
{
    addFile(file.name(), toDiffType(file.status()));
}

void DiffTreeModel::addFile(const QString &file, Diff::DiffType type)
{
    const auto parts = file.split(separator());
    TreeModel::Node *node;

    if (showRoot())
        node = createPath(QStringList() << separator() << parts, type);
    else
        node = createPath(parts, type);
    node->key = file;
    node->metaData = type;
}

TreeModel::Node *DiffTreeModel::createPath(const QStringList &path, Diff::DiffType status)
{
    Node *parent = mRootNode;
    for (auto &p : path) {
        auto child = parent->find(p);
        if (!child) {
            child = parent->createChild();
            child->title = p;
            child->metaData = Diff::DiffType::Unchanged;
        } else {
            if (status != Diff::DiffType::Unchanged && child->metaData != status)
                child->metaData = Diff::DiffType::Modified;
        }
        parent = child;
    }
    return parent;
}

QString icon(Diff::DiffType status)
{
    switch (status) {
    case Diff::DiffType::Added:
        return QStringLiteral("git-status-added");
    case Diff::DiffType::Modified:
        return QStringLiteral("git-status-modified");
    case Diff::DiffType::Removed:
        return QStringLiteral("git-status-removed");
    case Diff::DiffType::Unchanged:
        return QStringLiteral("git-status-update");
    }
    return QStringLiteral("git-status-update");
}

QColor textColor(Diff::DiffType status)
{
    switch (status) {
    case Diff::DiffType::Added:
        return GitKlientSettings::self()->diffAddedColor();
    case Diff::DiffType::Modified:
        return GitKlientSettings::self()->diffModifiedColor();
    case Diff::DiffType::Removed:
        return GitKlientSettings::self()->diffRemovedColor();
    case Diff::DiffType::Unchanged:
        return {};
    }
    return {};
}

QColor DiffTreeModel::statusColor(Diff::DiffType status) const
{
    switch (status) {
    case Diff::DiffType::Unchanged:
        return Qt::black;
    case Diff::DiffType::Added:
        return GitKlientSettings::diffAddedColor();
    case Diff::DiffType::Removed:
        return GitKlientSettings::diffRemovedColor();
    case Diff::DiffType::Modified:
        return GitKlientSettings::diffModifiedColor();
    }
    return {};
}

Diff::DiffType DiffTreeModel::toDiffType(FileStatus::Status status)
{
    switch (status) {
    case FileStatus::NoGit:
    case FileStatus::Unknown:
    case FileStatus::Unmodified:
        return Diff::DiffType::Unchanged;
    case FileStatus::Added:
        return Diff::DiffType::Added;
    case FileStatus::Removed:
        return Diff::DiffType::Removed;
    case FileStatus::Modified:
    case FileStatus::Renamed:
    case FileStatus::Copied:
        return Diff::DiffType::Modified;
    case FileStatus::UpdatedButInmerged:
    case FileStatus::Ignored:
    case FileStatus::Untracked:
        return Diff::DiffType::Unchanged;
    }
    return Diff::DiffType::Unchanged;
}

QVariant DiffTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (role == Qt::DisplayRole) {
        Node *item = static_cast<Node *>(index.internalPointer());

        switch (index.column()) {
        case 0:
            return item->title;
        case 1:
            return (int)item->metaData;
        }
    } else if (role == Qt::DecorationRole) {
        Node *item = static_cast<Node *>(index.internalPointer());

        //        return statusColor(item->metaData);

        //        switch (item->metaData) {
        //        case Diff::DiffType::Added:
        //            return QColor(Qt::green);
        //        case Diff::DiffType::Removed:
        //            return QColor(Qt::red);
        //        case Diff::DiffType::Modified:
        //            return QColor(Qt::blue);
        //        case Diff::DiffType::Unchanged:
        //            return QColor(Qt::black);
        //        }

        return QIcon::fromTheme(icon(item->metaData));
    } else if (role == Qt::ForegroundRole) {
        //        Node *item = static_cast<Node *>(index.internalPointer());
        //        return textColor(item->metaData);
    }

    return {};
}

void DiffTreeModel::emitAll()
{
    beginResetModel();
    endResetModel();
}
