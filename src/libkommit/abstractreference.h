#pragma once

#include <git2/types.h>

#include <QSharedPointer>

namespace Git
{

class AbstractReference
{
public:
    AbstractReference();
    virtual ~AbstractReference();

    virtual QSharedPointer<git_tree> tree() const = 0;
};

}
