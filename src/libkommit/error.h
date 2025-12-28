/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QObject>
#include <QString>
#include <git2/errors.h>

namespace Git
{

class LIBKOMMIT_EXPORT Error
{
    Q_GADGET

public:
    Error();

    enum Type {
        None = GIT_ERROR_NONE,
        Nomemory = GIT_ERROR_NOMEMORY,
        Os = GIT_ERROR_OS,
        Invalid = GIT_ERROR_INVALID,
        Reference = GIT_ERROR_REFERENCE,
        Zlib = GIT_ERROR_ZLIB,
        Repository = GIT_ERROR_REPOSITORY,
        Config = GIT_ERROR_CONFIG,
        Regex = GIT_ERROR_REGEX,
        Odb = GIT_ERROR_ODB,
        Index = GIT_ERROR_INDEX,
        Object = GIT_ERROR_OBJECT,
        Net = GIT_ERROR_NET,
        Tag = GIT_ERROR_TAG,
        Tree = GIT_ERROR_TREE,
        Indexer = GIT_ERROR_INDEXER,
        Ssl = GIT_ERROR_SSL,
        Submodule = GIT_ERROR_SUBMODULE,
        Thread = GIT_ERROR_THREAD,
        Stash = GIT_ERROR_STASH,
        Checkout = GIT_ERROR_CHECKOUT,
        Fetchhead = GIT_ERROR_FETCHHEAD,
        Merge = GIT_ERROR_MERGE,
        Ssh = GIT_ERROR_SSH,
        Filter = GIT_ERROR_FILTER,
        Revert = GIT_ERROR_REVERT,
        Callback = GIT_ERROR_CALLBACK,
        Cherrypick = GIT_ERROR_CHERRYPICK,
        Describe = GIT_ERROR_DESCRIBE,
        Rebase = GIT_ERROR_REBASE,
        Filesystem = GIT_ERROR_FILESYSTEM,
        Patch = GIT_ERROR_PATCH,
        Worktree = GIT_ERROR_WORKTREE,
        Sha = GIT_ERROR_SHA,
        Http = GIT_ERROR_HTTP,
        Internal = GIT_ERROR_INTERNAL,
        Graft = GIT_ERROR_GRAFTS
    };
    Q_ENUM(Type)

    Type type() const;
    QString typeString() const;

    QString message() const;

    static int lastType();
    static QString lastTypeString();
    static QString lastMessage();

private:
    Type _type;
    QString _message;
};

}
