/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "treemodel.h"
#include <KLocalizedString>
#include <QVariant>

#include "libkommitwidgets_appdebug.h"

class TreeModelPrivate
{
    TreeModel *q_ptr;
    Q_DECLARE_PUBLIC(TreeModel)

public:
    explicit TreeModelPrivate(TreeModel *parent);

    QString separator{QStringLiteral("/")};
    QIcon defaultIcon;
    bool showRoot{true};

    TreeNode *rootNode{new TreeNode};
    void sortNode(TreeNode *node);
};
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

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const TreeModel);
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeNode *parentNode;

    if (!parent.isValid())
        parentNode = d->rootNode;
    else
        parentNode = static_cast<TreeNode *>(parent.internalPointer());

    TreeNode *childNode = parentNode->children.value(row);
    if (childNode)
        return createIndex(row, column, childNode);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    Q_D(const TreeModel);

    TreeNode *childNode = static_cast<TreeNode *>(index.internalPointer());
    TreeNode *parentNode = childNode->parentNode();

    if (parentNode == d->rootNode)
        return QModelIndex();

    return createIndex(parentNode->row(), 0, parentNode);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const TreeModel);
    TreeNode *parentNode;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentNode = d->rootNode;
    else
        parentNode = static_cast<TreeNode *>(parent.internalPointer());

    return parentNode->children.size();
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

    if (role == Qt::DecorationRole)
        return d->defaultIcon;

    if (role != Qt::DisplayRole)
        return {};

    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());

    return node->title;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return i18n("Tree");

    return {};
}

TreeNode *TreeModel::createPath(const QStringList &path)
{
    Q_D(TreeModel);
    auto parent = d->rootNode;
    QString childPath;
    for (const auto &p : path) {
        auto child = parent->find(p);
        if (!child) {
            child = parent->appendChild();
            child->title = p;
            child->path = childPath;
        }
        if (!childPath.isEmpty())
            childPath.append(d->separator);
        childPath.append(p);
        parent = child;
    }
    return parent;
}

void TreeModel::addItem(const QString &p, NodeData *data)
{
    Q_D(TreeModel);

    auto path = QString{p}.remove(QLatin1Char('\r')).remove(QLatin1Char('\n')).trimmed(); // TODO: Why do we need this?

    TreeNode *node;

    auto nodePath = path;

    auto parts = nodePath.split(d->separator);

    if (d->showRoot && !path.isEmpty())
        parts.prepend("");

    node = createPath(parts);

    if (node && data) {
        node->nodeData = data;
    }
}

void TreeModel::addItems(QMap<QString, NodeData *> items)
{
    beginResetModel();
    for (auto i = items.begin(); i != items.end(); ++i) {
        addItem(i.key(), i.value());
    }
    endResetModel();
}

void TreeModel::addItems(const QStringList &items)
{
    beginResetModel();
    for (auto const &item : items)
        addItem(item);

    endResetModel();
}

void TreeModel::setShowRoot(bool show)
{
    Q_D(TreeModel);
    d->showRoot = show;
}

void TreeModel::setRootTitle(const QString &title)
{
    Q_D(TreeModel);
    d->rootNode->title = title;
}

QString TreeModel::fullPath(const QModelIndex &index) const
{
    Q_D(const TreeModel);
    auto node = static_cast<TreeNode *>(index.internalPointer());
    return node->path + d->separator + node->title;
}

TreeNode *TreeModel::rootNode() const
{
    Q_D(const TreeModel);
    return d->rootNode;
}

TreeNode *TreeModel::node(const QModelIndex &index) const
{
    if (!index.isValid())
        return nullptr;
    return static_cast<TreeNode *>(index.internalPointer());
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

void TreeModel::clear()
{
    Q_D(TreeModel);
    beginRemoveRows(QModelIndex(), 0, d->rootNode->children.size() - 1);
    delete d->rootNode;
    d->rootNode = new TreeNode{};
    endRemoveRows();
}

void TreeModel::sortItems()
{
    Q_D(TreeModel);
    beginResetModel();
    d->sortNode(d->rootNode);
    endResetModel();
}

void TreeModel::emitReset()
{
    beginResetModel();
    endResetModel();
}
TreeModelPrivate::TreeModelPrivate(TreeModel *parent)
    : q_ptr{parent}
{
}

void TreeModelPrivate::sortNode(TreeNode *node)
{
    qCDebug(KOMMIT_WIDGETS_LOG) << "Sorting" << node->title;
    std::sort(node->children.begin(), node->children.end(), [](TreeNode *l, TreeNode *r) {
        if (l->children.empty() && !r->children.empty())
            return false;
        if (!l->children.empty() && r->children.empty())
            return true;
        return l->title < r->title;
    });
    for (auto &n : node->children)
        sortNode(n);
}

#include "moc_treemodel.cpp"
