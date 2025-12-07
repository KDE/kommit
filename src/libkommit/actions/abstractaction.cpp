/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractaction.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

namespace Git
{

class Worker : public QRunnable
{
public:
    Worker(AbstractAction* action)
    {

    }

    void run ()override
    {
        _action->run();
    }

private:
    AbstractAction *_action;
};

AbstractAction::AbstractAction(QObject *parent)
    : QObject{parent}
{
}

bool AbstractAction::run()
{
    auto retCode = exec();
    Q_EMIT finished(0 == retCode);
    return 0 == retCode;
}

void AbstractAction::runAsync()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QtConcurrent::run(this, &AbstractAction::run);
#else
    QtConcurrent::run(&AbstractAction::run, this);
    // auto w = new Worker{this};
    // QThreadPool::globalInstance()->start(w);
#endif
}

}
