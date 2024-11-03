/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <Kommit/Tree>

namespace Git
{

class LIBKOMMIT_EXPORT ITree
{
public:
    enum class TreeType {
        Commit,
        Branch
    };

    virtual ~ITree();
    virtual Tree tree() const = 0;
    virtual QString treeTitle() const = 0;
};

inline ITree::~ITree()
{
}

}
