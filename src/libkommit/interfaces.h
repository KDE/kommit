/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>

#include "libkommit_export.h"

namespace Git
{

class Tree;
class Oid;

class LIBKOMMIT_EXPORT ITree
{
public:
    enum class TreeType { Commit, Branch };

    virtual ~ITree();
    virtual QSharedPointer<Tree> tree() const = 0;
    virtual QString treeTitle() const = 0;
};

inline ITree::~ITree()
{
}

class LIBKOMMIT_EXPORT IOid
{
public:
    virtual QSharedPointer<Oid> oid() const = 0;
};
}
