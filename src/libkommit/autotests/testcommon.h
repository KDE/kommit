/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <QString>

#pragma once

namespace Git
{
class Manager;
};

namespace TestCommon
{
bool touch(const QString &fileName);
QString readFile(const QString &fileName);
QString getTempPath();
bool cleanPath(Git::Manager *manager);
bool makePath(Git::Manager *manager, const QString &path);
}
