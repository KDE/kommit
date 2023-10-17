/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "testcommon.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QUuid>
#include <gitmanager.h>

namespace TestCommon
{

QString touch(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return {};
    auto content = QUuid::createUuid().toString(QUuid::Id128);
    f.write(content.toLatin1());
    f.close();
    return content;
}

QString getTempPath()
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1Char('/') + QUuid::createUuid().toString(QUuid::Id128);
    QDir d;
    d.mkpath(path);
    return path;
}

bool cleanPath(Git::Manager *manager)
{
    if (!manager->isValid())
        return false;
    QDir d{manager->path()};
    return d.removeRecursively();
}

QString readFile(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    auto buffer = f.readAll();
    f.close();
    return buffer;
}

bool makePath(Git::Manager *manager, const QString &path)
{
    QDir d{};
    return d.mkpath(manager->path() + "/" + path);
}
}
