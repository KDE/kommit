#pragma once

#include "abstractaction.h"

namespace Git
{

class PullPrivate;
class Pull : public AbstractAction
{
public:
    explicit Pull(QObject *parent = nullptr);

protected:
    int exec();

private:
    QSharedPointer<PullPrivate> d;

};
}
