/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QAbstractItemModel>

#include "libkommitwidgets_export.h"

#include <QIcon>
#include <QScopedPointer>

class TreeModelPrivate;

struct NodeData {
};

class TreeNode
{
public:
    explicit TreeNode(TreeNode *parent = nullptr);

    ~TreeNode();

    TreeNode *appendChild();

    int row() const;

    TreeNode *find(const QString &key);

    TreeNode *parentNode()
    {
        return parent;
    }

    QString path;
    QString title;
    NodeData *nodeData{nullptr};
    QVector<TreeNode *> children;
    TreeNode *parent;
};

inline TreeNode::TreeNode(TreeNode *parent)
    : parent(parent)
{
}

inline TreeNode::~TreeNode()
{
    qDeleteAll(children);
}

inline TreeNode *TreeNode::appendChild()
{
    auto ch = new TreeNode{this};
    ch->path = path + "/" + title;
    children.append(ch);
    return ch;
}

inline int TreeNode::row() const
{
    if (parent)
        return parent->children.indexOf(const_cast<TreeNode *>(this));
    return 0;
}

inline TreeNode *TreeNode::find(const QString &key)
{
    auto i = std::find_if(children.begin(), children.end(), [&key](TreeNode *node) {
        return node->title == key;
    });

    if (i == children.end())
        return nullptr;
    return *i;
}

class TreeModelPrivate;
class LIBKOMMITWIDGETS_EXPORT TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel();

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void addItem(const QString &path, NodeData *data = nullptr);
    void addItems(QMap<QString, NodeData *> items);
    void addItems(const QStringList &items);

    void setShowRoot(bool show);
    void setRootTitle(const QString &title);
    QString fullPath(const QModelIndex &index) const;

    TreeNode *rootNode() const;
    TreeNode *node(const QModelIndex &index) const;
    template<class T>
    T *nodeData(const QModelIndex &index) const
    {
        auto n = node(index);
        if (!n)
            return nullptr;

        return static_cast<T *>(n->nodeData);
    }

    [[nodiscard]] const QString &separator() const;
    void setSeparator(const QString &newSeparator);

    [[nodiscard]] const QIcon &defaultIcon() const;

    void setDefaultIcon(const QIcon &newDefaultIcon);

    void clear();

    void sortItems();
    void emitReset();

private:
    QScopedPointer<TreeModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(TreeModel)
    TreeNode *createPath(const QStringList &path);
};
