/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "treemodel.h"

#include "libkommitwidgets_appdebug.h"
#include <QModelIndex>

#include <KLocalizedString>

class TreeModelPrivate
{
    TreeModel *q_ptr;
    Q_DECLARE_PUBLIC(TreeModel)

public:
    TreeModelPrivate(TreeModel *parent);

    TreeModel::Node *createPath(const QStringList &path);
    TreeModel::Node *find(QStringList &path, TreeModel::Node *node = nullptr);
    void getFullPath(QString &path, TreeModel::Node *node) const;
    void sortNode(TreeModel::Node *node);

    TreeModel::Node *const rootNode;
    QString separator{QStringLiteral("/")};
    bool lastPartAsData{false};
    QIcon defaultIcon;
    bool showRoot{false};
};

const QString &TreeModel::separator() const
{
    Q_D(const TreeModel);
    return d->separator;
}

void TreeModel::setSeparator(const QString &newSeparator)
{
    Q_D(TreeModel);
    d->separator = newSeparator;
}

bool TreeModel::lastPartAsData() const
{
    Q_D(const TreeModel);
    return d->lastPartAsData;
}

void TreeModel::setLastPartAsData(bool newLastPartAsData)
{
    Q_D(TreeModel);
    d->lastPartAsData = newLastPartAsData;
}

const QIcon &TreeModel::defaultIcon() const
{
    Q_D(const TreeModel);
    return d->defaultIcon;
}

void TreeModel::setDefaultIcon(const QIcon &newDefaultIcon)
{
    Q_D(TreeModel);
    d->defaultIcon = newDefaultIcon;
}

void TreeModel::clear()
{
    Q_D(TreeModel);
    beginRemoveRows(QModelIndex(), 0, d->rootNode->childs.count() - 1);
    qDeleteAll(d->rootNode->childs);
    d->rootNode->childs.clear();
    endRemoveRows();
}

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr{new TreeModelPrivate{this}}
{
}

TreeModel::~TreeModel()
{
    Q_D(TreeModel);
    delete d->rootNode;
    delete d;
}

QModelIndex TreeModel::index(const Node *node, int col) const
{
    if (node->parent)
        return index(node->row, col, index(node->parent, col));

    return index(node->row, col, QModelIndex());
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const TreeModel);
    Node *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = d->rootNode;
    else
        parentItem = static_cast<Node *>(parent.internalPointer());

    return parentItem->childs.count();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    Q_D(const TreeModel);

    if (!index.isValid())
        return {};

    if (role == Qt::DisplayRole && index.column() == 0) {
        Node *item = static_cast<Node *>(index.internalPointer());

        return item->title;
    } else if (role == Qt::DecorationRole) {
        return d->defaultIcon;
    }

    return {};
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const TreeModel);
    if (!hasIndex(row, column, parent))
        return {};

    Node *parentItem;

    if (!parent.isValid())
        parentItem = d->rootNode;
    else
        parentItem = static_cast<Node *>(parent.internalPointer());

    Node *childItem = parentItem->childs.at(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return {};
}

QModelIndex TreeModel::parent(const QModelIndex &child) const
{
    Q_D(const TreeModel);
    if (!child.isValid())
        return {};

    Node *childItem = static_cast<Node *>(child.internalPointer());
    Node *parentItem = childItem->parent;

    if (parentItem == d->rootNode)
        return {};

    return createIndex(parentItem->row, 0, parentItem);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return i18n("Name");
        else
            return i18n("Status");
    } else {
        return section + 1;
    }
}

QStringList TreeModel::rootData() const
{
    Q_D(const TreeModel);
    return d->rootNode->data;
}

QStringList TreeModel::data(const QModelIndex &index) const
{
    return static_cast<Node *>(index.internalPointer())->data;
}

QString TreeModel::fullPath(const QModelIndex &index) const
{
    Q_D(const TreeModel);

    QString path;

    if (index.isValid())
        d->getFullPath(path, static_cast<Node *>(index.internalPointer()));
    else
        d->getFullPath(path, d->rootNode);

    return path;
}

QString TreeModel::key(const QModelIndex &index) const
{
    auto node = static_cast<Node *>(index.internalPointer());
    if (node)
        return node->key;
    return {};
}

QString TreeModel::section(const QModelIndex &index) const
{
    auto node = static_cast<Node *>(index.internalPointer());
    if (node)
        return node->prefix;
    return {};
}

void TreeModel::sortItems()
{
    Q_D(TreeModel);
    beginResetModel();
    d->sortNode(d->rootNode);
    endResetModel();
}

void TreeModel::addData(const QStringList &data, const QString &prefix, bool split)
{
    Q_D(TreeModel);
    for (const auto &p : data) {
        auto path = p;
        path = path.remove(QLatin1Char('\r')).remove(QLatin1Char('\n')).trimmed();
        if (path.isEmpty())
            continue;

        TreeModel::Node *node;

        if (split) {
            auto nodePath = path;
            if (!prefix.isEmpty())
                nodePath.prepend(prefix + d->separator);

            auto parts = nodePath.split(d->separator);
            if (d->lastPartAsData) {
                auto data = parts.takeLast();
                if (d->showRoot)
                    node = d->createPath(QStringList() << d->separator << parts);
                else
                    node = d->createPath(parts);

                if (d->showRoot && node != d->rootNode)
                    node->data.append(data);
                auto nodePathParts = nodePath.split(d->separator);
                nodePathParts.takeLast();
                node->key = nodePathParts.join(d->separator);
            } else {
                node = d->createPath(parts);
                node->key = path;
            }
        } else {
            if (!prefix.isEmpty())
                node = d->createPath({prefix, path});
            else
                node = d->createPath({path});
            node->key = path;
        }
        if (node) {
            node->prefix = prefix;
        }
    }
    beginInsertRows(QModelIndex(), 0, d->rootNode->childs.count() - 1);
    endInsertRows();
}

TreeModel::Node *TreeModelPrivate::find(QStringList &path, TreeModel::Node *node)
{
    if (path.empty())
        return nullptr;

    auto ch = node->find(path.first());
    if (!ch)
        return nullptr;

    auto p = path;
    p.removeFirst();
    return find(p, ch);
}

TreeModel::Node *TreeModelPrivate::createPath(const QStringList &path)
{
    TreeModel::Node *parent = rootNode;
    for (const auto &p : path) {
        auto child = parent->find(p);
        if (!child) {
            child = parent->createChild();
            child->title = p;
        }
        parent = child;
    }
    return parent;
}

void TreeModelPrivate::getFullPath(QString &path, TreeModel::Node *node) const
{
    if (showRoot && node == rootNode)
        return;
    if (node) {
        if (path.isEmpty())
            path = node->title;
        else
            path.prepend(node->title + separator);

        if ((showRoot && node->parent->parent != rootNode) || (!showRoot && node->parent != rootNode)) {
            // path.prepend(mSeparator);
            getFullPath(path, node->parent);
        }
    }
}

void TreeModelPrivate::sortNode(TreeModel::Node *node)
{
    qCDebug(KOMMIT_WIDGETS_LOG) << "Sorting" << node->title;
    std::sort(node->childs.begin(), node->childs.end(), [](TreeModel::Node *l, TreeModel::Node *r) {
        if (l->childs.empty() && !r->childs.empty())
            return false;
        if (!l->childs.empty() && r->childs.empty())
            return true;
        return l->title < r->title;
    });
    for (auto &n : node->childs)
        sortNode(n);
}

bool TreeModel::showRoot() const
{
    Q_D(const TreeModel);
    return d->showRoot;
}

void TreeModel::setShowRoot(bool newDefaultRoot)
{
    Q_D(TreeModel);
    d->showRoot = newDefaultRoot;
}

TreeModel::Node *TreeModel::rootNode() const
{
    Q_D(const TreeModel);
    return d->rootNode;
}

TreeModelPrivate::TreeModelPrivate(TreeModel *parent)
    : q_ptr{parent}
    , rootNode{new TreeModel::Node}
{
}

#include "moc_treemodel.cpp"
