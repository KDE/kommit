/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>
#include <QString>

#include <git2.h>

namespace Git
{

class ConfigEntry
{
public:
    ConfigEntry();
    explicit ConfigEntry(git_config_entry *entry);
    ConfigEntry(const ConfigEntry &other);

    ConfigEntry &operator=(const ConfigEntry &other);

    bool operator==(const ConfigEntry &other);
    bool operator!=(const ConfigEntry &other);

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

    [[nodiscard]] QString name() const;
    [[nodiscard]] QString value() const;
    [[nodiscard]] QString backendType() const;
    [[nodiscard]] QString originPath() const;
    [[nodiscard]] int includeDepth() const;
    [[nodiscard]] Level level() const;

private:
    QSharedPointer<git_config_entry> d;
};

class Config
{
public:
    explicit Config(git_config *config);

    [[nodiscard]] ConfigEntry value(const QString &name) const;
    [[nodiscard]] qint32 valueInt32(const QString &name) const;
    [[nodiscard]] int64_t valueInt64(const QString &name) const;
    [[nodiscard]] QString valueString(const QString &name) const;
    [[nodiscard]] bool valueBool(const QString &name) const;

private:
    QSharedPointer<git_config> d;
};

}
