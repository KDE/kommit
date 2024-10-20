/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/submodule.h>
#include <git2/types.h>

#include <QObject>
#include <QSharedPointer>
#include <QString>

#include <Kommit/Oid>

namespace Git
{

class FetchOptions;

class FetchObserver;

class Repository;
class SubmodulePrivate;
class LIBKOMMIT_EXPORT Submodule
{
    Q_GADGET

public:
    enum class Status {
        Unknown = 0,
        InHead = GIT_SUBMODULE_STATUS_IN_HEAD, // superproject head contains submodule
        InIndex = GIT_SUBMODULE_STATUS_IN_INDEX, // superproject index contains submodule
        InConfig = GIT_SUBMODULE_STATUS_IN_CONFIG, // superproject gitmodules has submodule
        InWd = GIT_SUBMODULE_STATUS_IN_WD, // superproject workdir has submodule
        IndexAdded = GIT_SUBMODULE_STATUS_INDEX_ADDED, // in index, not in head
        IndexDeleted = GIT_SUBMODULE_STATUS_INDEX_DELETED, // in head, not in index
        IndexModified = GIT_SUBMODULE_STATUS_INDEX_MODIFIED, // index and head don't match
        WdUninitialized = GIT_SUBMODULE_STATUS_WD_UNINITIALIZED, // workdir contains empty directory
        WdAdded = GIT_SUBMODULE_STATUS_WD_ADDED, // in workdir, not index
        WdDeleted = GIT_SUBMODULE_STATUS_WD_DELETED, // in index, not workdir
        WdModified = GIT_SUBMODULE_STATUS_WD_MODIFIED, // index and workdir head don't match
        WdIndexModified = GIT_SUBMODULE_STATUS_WD_INDEX_MODIFIED, // submodule workdir index is dirty
        WdWdModified = GIT_SUBMODULE_STATUS_WD_WD_MODIFIED, // submodule workdir has modified files
        WdUntracked = GIT_SUBMODULE_STATUS_WD_UNTRACKED, // wd contains untracked files
    };

    Q_DECLARE_FLAGS(StatusFlags, Status)
    Q_FLAG(StatusFlags)

    Submodule(git_submodule *submodule = nullptr, git_repository *repo = nullptr);
    Submodule(const Submodule &other);
    Submodule &operator=(const Submodule &other);
    bool operator==(const Submodule &other) const;
    bool operator!=(const Submodule &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_submodule *data() const;

    [[nodiscard]] const git_submodule *constData() const;
    [[nodiscard]] const QString &path() const;
    [[nodiscard]] const QString &refName() const;
    [[nodiscard]] QString url() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString branch();
    [[nodiscard]] StatusFlags status() const;

    [[nodiscard]] bool hasModifiedFiles() const;

    [[nodiscard]] Oid headId();
    [[nodiscard]] Oid indexId();
    [[nodiscard]] Oid workingDirectoryId();

    void setUrl(const QString &newUrl);
    bool sync() const;
    bool reload(bool force = false) const;

    [[nodiscard]] Repository *open() const;
    bool update(const FetchOptions &opts, FetchObserver *observer = nullptr);

private:
    QSharedPointer<SubmodulePrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Submodule::StatusFlags)

} // namespace Git
