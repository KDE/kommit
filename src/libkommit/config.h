/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>
#include <QString>
#include <git2/config.h>
#include <git2/version.h>

namespace Git
{

class ConfigEntry
{
public:
    ConfigEntry(git_config_entry *entry);
    ~ConfigEntry();
    enum class Level {
        Programdata = GIT_CONFIG_LEVEL_PROGRAMDATA,
        System = GIT_CONFIG_LEVEL_SYSTEM,
        Xdg = GIT_CONFIG_LEVEL_XDG,
        Global = GIT_CONFIG_LEVEL_GLOBAL,
        Local = GIT_CONFIG_LEVEL_LOCAL,
        App = GIT_CONFIG_LEVEL_APP,
        HighestLevel = GIT_CONFIG_HIGHEST_LEVEL,
#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) >= QT_VERSION_CHECK(1, 8, 0)
        Worktree = GIT_CONFIG_LEVEL_WORKTREE,
#endif
    };

    QString name() const;
    QString value() const;
    QString backendType() const;
    QString originPath() const;
    int includeDepth() const;
    Level level() const;

private:
    git_config_entry *mEntryPtr;
};

class Config
{
public:
    Config(git_config *config);

    QSharedPointer<ConfigEntry> value(const QString &name) const;
    qint32 valueInt32(const QString &name) const;
    int64_t valueInt64(const QString &name) const;
    QString valueString(const QString &name) const;
    bool valueBool(const QString &name) const;

private:
    git_config *mConfigPtr;
};

}
