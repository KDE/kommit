/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

#include "libkommitTestsCommon_global.h"
#include <QString>

namespace Git
{
class Repository;
}

namespace TestCommon
{
LIBKOMMITTESTSCOMMON_EXPORT void initSignature(Git::Repository *manager);

LIBKOMMITTESTSCOMMON_EXPORT QString touch(const QString &fileName);
LIBKOMMITTESTSCOMMON_EXPORT QString readFile(const QString &fileName);
LIBKOMMITTESTSCOMMON_EXPORT QString getTempPath(bool create = true);
LIBKOMMITTESTSCOMMON_EXPORT bool writeFile(Git::Repository *manager, const QString &fileName, const QString &content);
LIBKOMMITTESTSCOMMON_EXPORT bool cleanPath(Git::Repository *manager);

LIBKOMMITTESTSCOMMON_EXPORT QString touch(Git::Repository *manager, const QString &fileName);
LIBKOMMITTESTSCOMMON_EXPORT bool makePath(Git::Repository *manager, const QString &path);
LIBKOMMITTESTSCOMMON_EXPORT bool extractSampleRepo(const QString &path);
LIBKOMMITTESTSCOMMON_EXPORT QString runGit(const QString &workingDir, const QStringList &args);
}
