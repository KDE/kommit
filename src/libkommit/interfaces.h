#pragma once

#include <QSharedPointer>

#include "libkommit_export.h"

namespace Git
{

class Tree;
class LIBKOMMIT_EXPORT ITree
{
public:
    virtual ~ITree();
    virtual QSharedPointer<Tree> tree() const = 0;
};

inline ITree::~ITree()
{
}

}
