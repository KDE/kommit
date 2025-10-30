/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include "libkommit_export.h"
#include <Kommit/Repository>

namespace Git
{

class Error;
class LIBKOMMIT_EXPORT AbstractAction : public QObject
{
    Q_OBJECT

public:
    explicit AbstractAction(QObject *parent = nullptr);

    bool run();
    void runAsync();

protected:
    void setEror(const Error &err);
    virtual int exec() = 0;
    Repository *repo;

Q_SIGNALS:
    void finished(bool success);

    friend class Worker;
};

}
