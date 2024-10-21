/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "config.h"

namespace Git
{

ConfigEntry::ConfigEntry(git_config_entry *entry)
    : d{entry, git_config_entry_free}
{
}

QString ConfigEntry::name() const
{
    return QString{d->name};
}

QString ConfigEntry::value() const
{
    return QString{d->value};
}

QString ConfigEntry::backendType() const
{
#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) >= QT_VERSION_CHECK(1, 8, 0)
    return QString{d->backend_type};
#else
    return {};
#endif
}

QString ConfigEntry::originPath() const
{
#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) >= QT_VERSION_CHECK(1, 8, 0)
    return QString{d->origin_path};
#else
    return {};
#endif
}

int ConfigEntry::includeDepth() const
{
    return d->include_depth;
}

ConfigEntry::Level ConfigEntry::level() const
{
    return static_cast<Level>(d->level);
}
ConfigEntry::ConfigEntry(const ConfigEntry &other)
    : d{other.d}
{
}

ConfigEntry &ConfigEntry::operator=(const ConfigEntry &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}
bool ConfigEntry::operator==(const ConfigEntry &other)
{
    return d.data() == other.d.data();
}
bool ConfigEntry::operator!=(const ConfigEntry &other)
{
    return !(*this == other);
}
ConfigEntry::ConfigEntry()
    : d{nullptr}
{
}

Config::Config(git_config *config)
    : d{config, git_config_free}
{
}

ConfigEntry Config::value(const QString &name) const
{
    git_config_entry *entry;
    if (git_config_get_entry(&entry, d.data(), name.toLatin1().data()))
        return {};

    return ConfigEntry{entry};
}

qint32 Config::valueInt32(const QString &name) const
{
    qint32 ret{};
    git_config_get_int32(&ret, d.data(), name.toUtf8().constData());
    return ret;
}

int64_t Config::valueInt64(const QString &name) const
{
    int64_t ret{};
    git_config_get_int64(&ret, d.data(), name.toUtf8().constData());
    return ret;
}

QString Config::valueString(const QString &name) const
{
    const char *ret;
    git_config_get_string(&ret, d.data(), name.toUtf8().constData());
    return QString{ret};
}

bool Config::valueBool(const QString &name) const
{
    int ret{};
    git_config_get_bool(&ret, d.data(), name.toUtf8().constData());
    return ret;
}

}
