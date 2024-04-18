/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QAbstractListModel>

namespace Git
{
class Manager;
};

class LIBKOMMIT_EXPORT AbstractReport : public QAbstractListModel
{
    Q_OBJECT

protected:
    Git::Manager *mGit;

public:
    AbstractReport(Git::Manager *git, QObject *parent = nullptr);
    virtual ~AbstractReport();

    virtual void reload() = 0;
    virtual QString name() const = 0;
};
