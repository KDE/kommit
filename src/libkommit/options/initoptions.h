/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/repository.h>

#include <Kommit/FileMode>

#include <QSharedPointer>

namespace Git
{

class InitOptionsPrivate;
class LIBKOMMIT_EXPORT InitOptions
{
public:
    enum InitFlag {
        None = 0,
        Bare = GIT_REPOSITORY_INIT_BARE,
        NoReinit = GIT_REPOSITORY_INIT_NO_REINIT,
        Mkdir = GIT_REPOSITORY_INIT_MKDIR,
        Mkpath = GIT_REPOSITORY_INIT_MKPATH,
        ExternalTemplate = GIT_REPOSITORY_INIT_EXTERNAL_TEMPLATE,
        RelativeGitlink = GIT_REPOSITORY_INIT_RELATIVE_GITLINK,
    };
    Q_DECLARE_FLAGS(InitFlags, InitFlag)

    InitOptions();

    void apply(git_repository_init_options *options);

    FileMode mode() const;
    void setMode(const FileMode &newMode);

    QString description() const;
    void setDescription(const QString &newDescription);

    QString workdirPath() const;
    void setWorkdirPath(const QString &newWorkdirPath);

    QString templatePath() const;
    void setTemplatePath(const QString &newTemplatePath);

    QString initialHead() const;
    void setInitialHead(const QString &newInitialHead);

    QString originUrl() const;
    void setOriginUrl(const QString &newOriginUrl);

    InitFlags flags() const;
    void setFlags(const InitFlags &newFlags);

private:
    QSharedPointer<InitOptionsPrivate> d;
};
}
