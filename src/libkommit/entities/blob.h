/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/index.h>
#include <git2/types.h>

#include <QSharedPointer>
#include <QString>
#include <QScopedPointer>

namespace Git
{

class Oid;
class Repository;
class BlobPrivate;
class LIBKOMMIT_EXPORT Blob
{
public:
    explicit Blob(git_blob *blob);
    Blob(git_repository *repo, git_tree_entry *entry);
    Blob(git_repository *repo, const git_index_entry *entry);
    Blob(git_repository *repo, const QString &relativePath);
    explicit Blob(Repository *git, QSharedPointer<Oid> oid);
    virtual ~Blob();

    [[nodiscard]] const QString &name() const;
    bool save(const QString &path) const;
    Q_REQUIRED_RESULT QString saveAsTemp() const;
    QString stringContent() const;
    QByteArray content() const;

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isBinary() const;
    [[nodiscard]] quint64 size() const;

    QSharedPointer<Oid> oid() const;
    [[nodiscard]] QString fileName() const;
    [[nodiscard]] QString filePath() const;

    [[nodiscard]] static QSharedPointer<Blob> lookup(git_repository *repo, const QString &place, const QString &filePath);
    [[nodiscard]] static QSharedPointer<Blob> fromDisk(git_repository *repo, const QString &filePath);

private:
    QScopedPointer<BlobPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Blob)
};

}
