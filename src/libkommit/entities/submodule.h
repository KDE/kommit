/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/types.h>

#include <QObject>
#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT Submodule
{
    Q_GADGET

public:
    enum class Status {
        Unknown = 0,
        InHead = (1u << 0), // 1
        InIndex = (1u << 1), // 2
        InConfig = (1u << 2), // 4
        InWd = (1u << 3), // 8
        IndexAdded = (1u << 4), // 16
        IndexDeleted = (1u << 5),
        IndexModified = (1u << 6),
        WdUninitialized = (1u << 7),
        WdAdded = (1u << 8),
        WdDeleted = (1u << 9),
        WdModified = (1u << 10),
        WdIndexModified = (1u << 11),
        WdWdModified = (1u << 12),
        WdUntracked = (1u << 13),
    };
    Q_DECLARE_FLAGS(StatusFlags, Status)
    Q_FLAG(StatusFlags)

    Submodule();
    Submodule(git_submodule *submodule);
    ~Submodule();

    Q_REQUIRED_RESULT const QString &path() const;
    Q_REQUIRED_RESULT const QString &commitHash() const;
    Q_REQUIRED_RESULT const QString &refName() const;
    Q_REQUIRED_RESULT QString url() const;
    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString branch();
    Q_REQUIRED_RESULT StatusFlags status() const;

    void setUrl(const QString &newUrl);
    bool sync() const;
    bool reload(bool force = false) const;

private:
    git_submodule *ptr;
    QString mName;
    QString mUrl;
    QString mPath;
    QString mCommitHash;
    QString mRefName;
    QString mBranch;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Submodule::StatusFlags)

} // namespace Git
