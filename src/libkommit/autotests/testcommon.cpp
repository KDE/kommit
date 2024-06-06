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
    QFileInfo fi{fileName};
    fi.absoluteDir().mkpath(fi.absolutePath());

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
    const auto path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1Char('/') + QUuid::createUuid().toString(QUuid::Id128);
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
    const auto buffer = f.readAll();
    f.close();
    return buffer;
}

bool makePath(Git::Manager *manager, const QString &path)
{
    QDir d{};
    return d.mkpath(manager->path() + QLatin1Char('/') + path);
}

QString touch(Git::Manager *manager, const QString &fileName)
{
    QString content;
    if (fileName.startsWith(QLatin1Char('/')))
        content = touch(manager->path() + fileName);
    else
        content = touch(manager->path() + QLatin1Char('/') + fileName);
    manager->addFile(fileName);
    return content;
}

void initSignature(Git::Manager *manager)
{
    manager->setConfig("user.name", "kommit test user", Git::Manager::ConfigLocal);
    manager->setConfig("user.email", "kommit@kde.org", Git::Manager::ConfigLocal);
}
}
