/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QSharedPointer>
#include <QString>

#include <functional>

#include <git2.h>

namespace Git
{

enum class ConfigLevel {
    ProgramData = GIT_CONFIG_LEVEL_PROGRAMDATA,
    System = GIT_CONFIG_LEVEL_SYSTEM,
    Xdg = GIT_CONFIG_LEVEL_XDG,
    Global = GIT_CONFIG_LEVEL_GLOBAL,
    Local = GIT_CONFIG_LEVEL_LOCAL,
    App = GIT_CONFIG_LEVEL_APP,
    HighestLevel = GIT_CONFIG_HIGHEST_LEVEL,
    Worktree = GIT_CONFIG_LEVEL_WORKTREE,
};

class LIBKOMMIT_EXPORT ConfigEntry
{
public:
    ConfigEntry();
    explicit ConfigEntry(git_config_entry *entry);

    ConfigEntry(const ConfigEntry &other);

    ConfigEntry &operator=(const ConfigEntry &other);

    bool operator==(const ConfigEntry &other);
    bool operator!=(const ConfigEntry &other);

    [[nodiscard]] QString name() const;
    [[nodiscard]] QString value() const;
    [[nodiscard]] QString backendType() const;
    [[nodiscard]] QString originPath() const;
    [[nodiscard]] int includeDepth() const;
    [[nodiscard]] ConfigLevel level() const;

private:
    QSharedPointer<git_config_entry> d;
};

class LIBKOMMIT_EXPORT Config
{
public:
    explicit Config(git_config *config);

    [[nodiscard]] bool isNull() const;

    void set(const QString &name, bool value);
    void set(const QString &name, qint32 value);
    void set(const QString &name, qint64 value);
    void set(const QString &name, QStringView value);
    void set(const QString &name, const char *value);

    [[nodiscard]] ConfigEntry value(const QString &name) const;
    [[nodiscard]] bool valueBool(const QString &name) const;
    [[nodiscard]] qint32 valueInt32(const QString &name) const;
    [[nodiscard]] int64_t valueInt64(const QString &name) const;
    [[nodiscard]] QString valueString(const QString &name) const;
    [[nodiscard]] bool remove(const QString &name);

    void forEach(std::function<void(const QString &name, const QString &value)> calback);

private:
    QSharedPointer<git_config> d;
};

}
