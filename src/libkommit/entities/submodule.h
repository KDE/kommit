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

class Manager;
class LIBKOMMIT_EXPORT Submodule
{
    Q_GADGET

public:
    enum class Status {
        Unknown = 0,
        InHead = (1u << 0), // superproject head contains submodule
        InIndex = (1u << 1), // superproject index contains submodule
        InConfig = (1u << 2), // superproject gitmodules has submodule
        InWd = (1u << 3), // superproject workdir has submodule
        IndexAdded = (1u << 4), // in index, not in head
        IndexDeleted = (1u << 5), // in head, not in index
        IndexModified = (1u << 6), // index and head don't match
        WdUninitialized = (1u << 7), // workdir contains empty directory
        WdAdded = (1u << 8), // in workdir, not index
        WdDeleted = (1u << 9), // in index, not workdir
        WdModified = (1u << 10), // index and workdir head don't match
        WdIndexModified = (1u << 11), // submodule workdir index is dirty
        WdWdModified = (1u << 12), // submodule workdir has modified files
        WdUntracked = (1u << 13), // wd contains untracked files
    };
    Q_DECLARE_FLAGS(StatusFlags, Status)
    Q_FLAG(StatusFlags)

    Submodule();
    explicit Submodule(git_submodule *submodule);
    Submodule(git_repository *repo, git_submodule *submodule);
    ~Submodule();

    [[nodiscard]] const QString &path() const;
    [[nodiscard]] const QString &commitHash() const;
    [[nodiscard]] const QString &refName() const;
    [[nodiscard]] QString url() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString branch();
    [[nodiscard]] StatusFlags status() const;
    [[nodiscard]] QStringList statusTexts() const;

    void setUrl(const QString &newUrl);
    bool sync() const;
    bool reload(bool force = false) const;

    [[nodiscard]] Manager *open() const;

private:
    git_submodule *ptr = nullptr;
    git_repository *mRepo = nullptr;
    QString mName;
    QString mUrl;
    QString mPath;
    QString mCommitHash;
    QString mRefName;
    QString mBranch;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Submodule::StatusFlags)

} // namespace Git
