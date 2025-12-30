/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "initoptions.h"

namespace Git
{

class InitOptionsPrivate
{
public:
    FileMode mode;
    QString description;
    QString workdirPath;
    QString templatePath;
    QString initialHead;
    QString originUrl;
    InitOptions::InitFlags flags{InitOptions::None};
};

InitOptions::InitOptions()
    : d{new InitOptionsPrivate}
{
}

void InitOptions::apply(git_repository_init_options *options)
{
    git_repository_init_options_init(options, GIT_REPOSITORY_INIT_OPTIONS_VERSION);

    if (d->mode.isValid())
        options->mode = d->mode.value();

    if (!d->description.isEmpty())
        options->description = d->description.toUtf8().constData();
    if (!d->workdirPath.isEmpty())
        options->workdir_path = d->workdirPath.toUtf8().constData();
    if (!d->templatePath.isEmpty())
        options->template_path = d->templatePath.toUtf8().constData();
    if (!d->initialHead.isEmpty())
        options->initial_head = d->initialHead.toUtf8().constData();
    if (!d->originUrl.isEmpty())
        options->origin_url = d->originUrl.toUtf8().constData();
    options->flags = static_cast<quint32>(d->flags);
}

FileMode InitOptions::mode() const
{
    return d->mode;
}

void InitOptions::setMode(const FileMode &newMode)
{
    d->mode = newMode;
}

QString InitOptions::description() const
{
    return d->description;
}

void InitOptions::setDescription(const QString &newDescription)
{
    d->description = newDescription;
}

QString InitOptions::workdirPath() const
{
    return d->workdirPath;
}

void InitOptions::setWorkdirPath(const QString &newWorkdirPath)
{
    d->workdirPath = newWorkdirPath;
}

QString InitOptions::templatePath() const
{
    return d->templatePath;
}

void InitOptions::setTemplatePath(const QString &newTemplatePath)
{
    d->templatePath = newTemplatePath;
}

QString InitOptions::initialHead() const
{
    return d->initialHead;
}

void InitOptions::setInitialHead(const QString &newInitialHead)
{
    d->initialHead = newInitialHead;
}

QString InitOptions::originUrl() const
{
    return d->originUrl;
}

void InitOptions::setOriginUrl(const QString &newOriginUrl)
{
    d->originUrl = newOriginUrl;
}

InitOptions::InitFlags InitOptions::flags() const
{
    return d->flags;
}

void InitOptions::setFlags(const InitFlags &newFlags)
{
    d->flags = newFlags;
}
}
