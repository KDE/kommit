/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "config.h"

#include "gitglobal_p.h"

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
    return QString{d->backend_type};
}

QString ConfigEntry::originPath() const
{
    return QString{d->origin_path};
}

int ConfigEntry::includeDepth() const
{
    return d->include_depth;
}

ConfigLevel ConfigEntry::level() const
{
    return static_cast<ConfigLevel>(d->level);
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

bool Config::isNull() const
{
    return d.isNull();
}

void Config::set(const QString &name, bool value)
{
    if (Q_UNLIKELY(d.isNull()))
        return;
    git_config_set_bool(d.data(), name.toUtf8().constData(), value);
}

void Config::set(const QString &name, qint32 value)
{
    if (Q_UNLIKELY(d.isNull()))
        return;
    git_config_set_int64(d.data(), name.toUtf8().constData(), value);
}

void Config::set(const QString &name, qint64 value)
{
    if (Q_UNLIKELY(d.isNull()))
        return;
    git_config_set_int64(d.data(), name.toUtf8().constData(), value);
}

void Config::set(const QString &name, QStringView value)
{
    if (Q_UNLIKELY(d.isNull()))
        return;

    git_config_set_string(d.data(), name.toUtf8().constData(), value.toUtf8().constData());
}

void Config::set(const QString &name, const char *value)
{
    set(name, QString{value});
}

ConfigEntry Config::value(const QString &name) const
{
    if (Q_UNLIKELY(d.isNull()))
        return {};
    git_config_entry *entry;
    if (git_config_get_entry(&entry, d.data(), name.toLatin1().data()))
        return {};

    return ConfigEntry{entry};
}

qint32 Config::valueInt32(const QString &name) const
{
    if (Q_UNLIKELY(d.isNull()))
        return {};
    qint32 ret{};
    git_config_get_int32(&ret, d.data(), name.toUtf8().constData());
    return ret;
}

int64_t Config::valueInt64(const QString &name) const
{
    if (Q_UNLIKELY(d.isNull()))
        return {};
    int64_t ret{};
    git_config_get_int64(&ret, d.data(), name.toUtf8().constData());
    return ret;
}

QString Config::valueString(const QString &name) const
{
    if (Q_UNLIKELY(d.isNull()))
        return {};
    git_buf buf = GIT_BUF_INIT;
    int error = git_config_get_string_buf(&buf, d.data(), name.toUtf8().constData());

    if (error < 0) {
        // Key not found or other error
        return {};
    }

    QString result = QString::fromUtf8(buf.ptr, buf.size);
    git_buf_dispose(&buf);

    return result;
}

bool Config::valueBool(const QString &name) const
{
    if (Q_UNLIKELY(d.isNull()))
        return {};
    int ret{};
    git_config_get_bool(&ret, d.data(), name.toUtf8().constData());
    return ret;
}

bool Config::remove(const QString &name)
{
    if (Q_UNLIKELY(d.isNull()))
        return {};
    return 0 == git_config_delete_entry(d.data(), name.toUtf8().constData());
}

void Config::forEach(std::function<void(const QString &, const QString &)> calback)
{
    struct wrapper {
        std::function<void(const QString &, const QString &)> calback;
    };
    wrapper w;
    w.calback = calback;

    auto cb = [](const git_config_entry *entry, void *payload) -> int {
        auto w = reinterpret_cast<wrapper *>(payload);
        w->calback(QString{entry->name}, QString{entry->value});
        return 0;
    };

    git_config_foreach(d.data(), cb, &w);
}
}
