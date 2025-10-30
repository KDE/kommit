#pragma once

#include "abstractaction.h"

namespace Git
{

class MergePrivate;

class Merge : public AbstractAction
{
public:
    explicit Merge(QObject *parent = nullptr);

protected:
    int exec();

private:
    QSharedPointer<MergePrivate> d;
};
}
