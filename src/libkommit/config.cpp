/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "config.h"

#include <git2/config.h>
#include <git2/version.h>

namespace Git
{

ConfigEntry::ConfigEntry(git_config_entry *entry)
    : mEntryPtr{entry}
{
}

ConfigEntry::~ConfigEntry()
{
    git_config_entry_free(mEntryPtr);
}

QString ConfigEntry::name() const
{
    return QString{mEntryPtr->name};
}

QString ConfigEntry::value() const
{
    return QString{mEntryPtr->value};
}

QString ConfigEntry::backendType() const
{
#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) >= QT_VERSION_CHECK(1, 8, 0)
    return QString{mEntryPtr->backend_type};
#else
    return {};
#endif
}

QString ConfigEntry::originPath() const
{
#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) >= QT_VERSION_CHECK(1, 8, 0)
    return QString{mEntryPtr->origin_path};
#else
    return {};
#endif
}

int ConfigEntry::includeDepth() const
{
    return mEntryPtr->include_depth;
}

ConfigEntry::Level ConfigEntry::level() const
{
    return static_cast<Level>(mEntryPtr->level);
}

Config::Config(git_config *config)
    : mConfigPtr{config}
{
}

QSharedPointer<ConfigEntry> Config::value(const QString &name) const
{
    git_config_entry *entry;
    if (git_config_get_entry(&entry, mConfigPtr, name.toLatin1().data()))
        return nullptr;

    return QSharedPointer<ConfigEntry>{new ConfigEntry{entry}};
}

qint32 Config::valueInt32(const QString &name) const
{
    qint32 ret{};
    git_config_get_int32(&ret, mConfigPtr, name.toUtf8().constData());
    return ret;
}

int64_t Config::valueInt64(const QString &name) const
{
    int64_t ret{};
    git_config_get_int64(&ret, mConfigPtr, name.toUtf8().constData());
    return ret;
}

QString Config::valueString(const QString &name) const
{
    const char *ret;
    git_config_get_string(&ret, mConfigPtr, name.toUtf8().constData());
    return QString{ret};
}

bool Config::valueBool(const QString &name) const
{
    int ret{};
    git_config_get_bool(&ret, mConfigPtr, name.toUtf8().constData());
    return ret;
}

}
