/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "blob.h"

#include "entities/commit.h"
#include "entities/tree.h"
#include "gitglobal_p.h"
#include "object.h"
#include "oid.h"
#include "repository.h"

#include <git2/blob.h>
#include <git2/revparse.h>
#include <git2/tree.h>

#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUuid>

namespace Git
{

class BlobPrivate
{
    Blob *q_ptr;
    Q_DECLARE_PUBLIC(Blob)

public:
    explicit BlobPrivate(Blob *parent);
    ~BlobPrivate();

    QString path;
    git_blob *blob{nullptr};
    QString name;
};
BlobPrivate::BlobPrivate(Blob *parent)
    : q_ptr{parent}
{
}

BlobPrivate::~BlobPrivate()
{
    git_blob_free(blob);
}

Blob::Blob()
    : d{new BlobPrivate{this}}
{
}

Blob::Blob(git_blob *blob)
    : d{new BlobPrivate{this}}
{
    d->blob = blob;
}

Blob::Blob(git_repository *repo, git_tree_entry *entry, const QString &path)
    : d{new BlobPrivate{this}}
{
    d->name = QString{git_tree_entry_name(entry)};

    auto oid = git_tree_entry_id(entry);
    git_blob_lookup(&d->blob, repo, oid);
    d->path =path;
}

Blob::Blob(git_repository *repo, const git_index_entry *entry)
    : d{new BlobPrivate{this}}
{
    d->name = QString{entry->path};

    git_blob_lookup(&d->blob, repo, &entry->id);
    d->path = entry->path;
}

Blob::Blob(git_repository *repo, const QString &relativePath)
    : d{new BlobPrivate{this}}
{
    d->name = relativePath;
    git_oid oid;
    git_blob_create_from_workdir(&oid, repo, relativePath.toUtf8().data());
    git_blob_lookup(&d->blob, repo, &oid);
}

Blob::Blob(Repository *git, const Oid &oid)
{
    git_blob_lookup(&d->blob, git->repoPtr(), oid.constData());
}





const QString &Blob::name() const
{
    return d->name;
}

bool Blob::save(const QString &path) const
{
    QFile f{path};
    if (!f.open(QIODevice::WriteOnly))
        return false;

    f.write(content());
    f.close();
    return true;
}

QString Blob::saveAsTemp() const
{
    QFileInfo fi{filePath()};
    QString fileName;
    const auto tempLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (tempLocation.isEmpty())
        return {};

    fileName = QStringLiteral("%1/%2-%3.%4").arg(tempLocation, fi.baseName(), QUuid::createUuid().toString(QUuid::Id128), fi.suffix());

    if (save(fileName))
        return fileName;

    return {};
}

QString Blob::stringContent() const
{
    auto blob = d->blob;
    if (!blob)
        return {};

    QString ch = (char *)git_blob_rawcontent(blob);
    return ch;
}

QByteArray Blob::content() const
{
    auto blob = d->blob;
    if (!blob)
        return QByteArray{};

    QByteArray buffer{(char *)git_blob_rawcontent(blob)};
    git_blob_free(blob);
    return buffer;
}

bool Blob::isValid() const
{
    return d->blob;
}

bool Blob::isBinary() const
{
    return git_blob_is_binary(d->blob);
}

quint64 Blob::size() const
{
    return git_blob_rawsize(d->blob);
}

Oid Blob::oid() const
{
    return Oid{git_blob_id(d->blob)};
}

QString Blob::fileName() const
{
    return d->name.mid(d->name.lastIndexOf(QStringLiteral("/")) + 1);
}

QString Blob::filePath() const
{
    return d->path + "/" + d->name;
}

bool Blob::isNull() const
{
    return !d->blob;
}

Blob Blob::lookup(git_repository *repo, const QString &place, const QString &filePath)
{
    git_object *obj = nullptr;

    SequenceRunner r;
    r.run(git_revparse_single, &obj, repo, place.toUtf8().constData());

    if (r.isError())
        return Blob{};

    Object object{obj};
    Tree tree;

    switch (object.type()) {
    case Object::Type::Commit:
        tree = object.toCommit().tree();
        break;

    case Object::Type::Tree:
        tree = object.toTree();
        break;

    default:
        return Blob{};
    }

    if (tree.isNull())
        return Blob{};

    return tree.file(filePath);
}

Blob Blob::fromDisk(git_repository *repo, const QString &filePath)
{
    git_oid oid;
    git_blob *blob;
    SequenceRunner r;
    r.run(git_blob_create_from_disk, &oid, repo, filePath.toUtf8().data());
    r.run(git_blob_lookup, &blob, repo, &oid);
    if (r.isError())
        return Blob{};
    return Blob{blob};
}
}
