/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "difftreemodel.h"

#include <QSet>

struct DiffNodeData : public NodeData {
    explicit DiffNodeData(Diff::DiffType diffType);

    Diff::DiffType diffType;
};

DiffTreeModel::DiffTreeModel(QObject *parent)
    : TreeModel(parent)
{
    // setLastPartAsData(true);
}

void DiffTreeModel::addFile(const Git::FileStatus &file)
{
    addFile(file.name(), toDiffType(file.status()));
}

void DiffTreeModel::addFile(const QString &file, Diff::DiffType type)
{
    auto data = new DiffNodeData{type};

    addItem(file, data);
    // const auto parts = file.split(separator());
    // TreeNode *node;

    // if (showRoot())
    //     node = createPath(QStringList() << separator() << parts, type);
    // else
    //     node = createPath(parts, type);
    // node->key = file;
    // node->metaData = type;
}

void DiffTreeModel::addFile(const Git::TreeDiffEntry &file)
{
    addFile(file.newFile(), toDiffType(file.status()));
}

TreeNode *DiffTreeModel::createPath(const QStringList &path, Diff::DiffType status)
{
    auto parent = rootNode();
    for (auto &p : path) {
        auto child = parent->find(p);
        if (!child) {
            child = parent->appendChild();
            child->title = p;
            child->nodeData = new DiffNodeData{Diff::DiffType::Unchanged};
        } else {
            auto diffData = static_cast<DiffNodeData *>(child->nodeData);
            if (status != Diff::DiffType::Unchanged && diffData->diffType != status)
                static_cast<DiffNodeData *>(child->nodeData)->diffType = Diff::DiffType::Modified;
        }
        parent = child;
    }
    return parent;
}

QString icon(Diff::DiffType status)
{
    switch (status) {
    case Diff::DiffType::Added:
        return QStringLiteral(":/kommit/icons/status-added");
    case Diff::DiffType::Modified:
        return QStringLiteral(":/kommit/icons/status-modified");
    case Diff::DiffType::Removed:
        return QStringLiteral(":/kommit/icons/status-removed");
    case Diff::DiffType::Unchanged:
        return QStringLiteral(":/kommit/icons/status-update");
    }
    return QStringLiteral(":/kommit/icons/status-update");
}

Diff::DiffType DiffTreeModel::toDiffType(Git::FileStatus::Status status) const
{
    switch (status) {
    case Git::FileStatus::NoGit:
    case Git::FileStatus::Unknown:
    case Git::FileStatus::Unmodified:
        return Diff::DiffType::Unchanged;
    case Git::FileStatus::Added:
        return Diff::DiffType::Added;
    case Git::FileStatus::Removed:
        return Diff::DiffType::Removed;
    case Git::FileStatus::Modified:
    case Git::FileStatus::Renamed:
    case Git::FileStatus::Copied:
        return Diff::DiffType::Modified;
    case Git::FileStatus::UpdatedButInmerged:
    case Git::FileStatus::Ignored:
    case Git::FileStatus::Untracked:
        return Diff::DiffType::Unchanged;
    }
    return Diff::DiffType::Unchanged;
}

Diff::DiffType DiffTreeModel::toDiffType(Git::ChangeStatus status) const
{
    switch (status) {
    case Git::ChangeStatus::Unknown:
    case Git::ChangeStatus::Unmodified:
        return Diff::DiffType::Unchanged;
    case Git::ChangeStatus::Added:
        return Diff::DiffType::Added;
    case Git::ChangeStatus::Removed:
        return Diff::DiffType::Removed;
    case Git::ChangeStatus::Modified:
    case Git::ChangeStatus::Renamed:
    case Git::ChangeStatus::Copied:
        return Diff::DiffType::Modified;
    case Git::ChangeStatus::UpdatedButInmerged:
    case Git::ChangeStatus::Ignored:
    case Git::ChangeStatus::Untracked:
        return Diff::DiffType::Unchanged;
    case Git::ChangeStatus::TypeChange:
    case Git::ChangeStatus::Unreadable:
    case Git::ChangeStatus::Conflicted:
        break;
    }
    return Diff::DiffType::Unchanged;
}

Diff::DiffType DiffTreeModel::calculateNodeType(TreeNode *node) const
{
    QList<Diff::DiffType> set;

    std::function<bool(TreeNode *)> fn = [&set, &fn](TreeNode *node) -> bool {
        for (auto const &child : std::as_const(node->children)) {
            auto nodeDiffData = static_cast<DiffNodeData *>(child->nodeData);
            if (!nodeDiffData) {
                if (fn(child))
                    return true;
                continue;
            }

            if (!set.contains(nodeDiffData->diffType))
                set.append(nodeDiffData->diffType);
        }

        return false;
    };

    fn(node);

    switch (set.size()) {
    case 0:
        return Diff::DiffType::Unchanged;

    case 1:
        return set.first();

    default:
        return Diff::DiffType::Modified;
    }
}

QVariant DiffTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (role == Qt::DisplayRole) {
        auto item = static_cast<TreeNode *>(index.internalPointer());

        switch (index.column()) {
        case 0:
            return item->title;
        case 1:
            return static_cast<int>(static_cast<DiffNodeData *>(item->nodeData)->diffType);
        }
    } else if (role == Qt::DecorationRole) {
        auto item = static_cast<TreeNode *>(index.internalPointer());

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

        auto diffData = static_cast<DiffNodeData *>(item->nodeData);
        if (diffData)
            return QIcon::fromTheme(icon(diffData->diffType));
        else
            return QIcon::fromTheme(icon(calculateNodeType(item)));
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

DiffNodeData::DiffNodeData(Diff::DiffType diffType)
    : diffType(diffType)
{
}

#include "moc_difftreemodel.cpp"
