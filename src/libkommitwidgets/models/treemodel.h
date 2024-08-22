/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QAbstractItemModel>

#include "libkommitwidgets_export.h"
#include <QIcon>

class TreeModelPrivate;
class LIBKOMMITWIDGETS_EXPORT TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    struct Node {
        QString title;
        QList<Node *> childs;
        Node *parent;

        struct Feature *feature;
        int row{};
        QStringList data;
        QString key;
        QString prefix;
#ifdef TREEMODEL_NODE_DATA_TYPE
        TREEMODEL_NODE_DATA_TYPE metaData;
#endif
        Node()
            : parent(nullptr)
            , feature(nullptr)
        {
        }
        explicit Node(Node *parent)
            : parent(parent)
            , feature(nullptr)
        {
            row = parent->childs.count();
        }

        ~Node()
        {
            qDeleteAll(childs);
        }

        Node *createChild()
        {
            auto ch = new Node;
            ch->parent = this;
            ch->row = childs.count();
            childs.append(ch);
            return ch;
        }
        Node *find(const QString &title)
        {
            auto i = childs.begin();
            while (i != childs.end()) {
                if ((*i)->title == title)
                    return *i;
                ++i;
            }
            return nullptr;
        }

        void clear()
        {
            qDeleteAll(childs);
            childs.clear();
        }
    };

    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel() override;

    [[nodiscard]] QModelIndex index(const Node *node, int col) const;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &child) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] QStringList rootData() const;
    [[nodiscard]] QStringList data(const QModelIndex &index) const;
    [[nodiscard]] QString fullPath(const QModelIndex &index) const;
    [[nodiscard]] QString key(const QModelIndex &index) const;
    [[nodiscard]] QString section(const QModelIndex &index) const;
    void sortItems();

    void addData(const QStringList &data, const QString &prefix = QString(), bool split = true);

    [[nodiscard]] const QString &separator() const;
    void setSeparator(const QString &newSeparator);

    [[nodiscard]] bool lastPartAsData() const;
    void setLastPartAsData(bool newLastPartAsData);

    [[nodiscard]] const QIcon &defaultIcon() const;
    void setDefaultIcon(const QIcon &newDefaultIcon);

    void clear();

    [[nodiscard]] bool showRoot() const;
    void setShowRoot(bool newDefaultRoot);

protected:
    Node *rootNode() const;

private:
    TreeModelPrivate *d_ptr;
    Q_DECLARE_PRIVATE(TreeModel)
};
