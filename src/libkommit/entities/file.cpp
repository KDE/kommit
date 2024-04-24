/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/file.h"
#include "../gitmanager.h"

#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUuid>
#include <utility>

#include "types.h"

namespace Git
{

File::StorageType File::storage() const
{
    return mStorage;
}

File::File()
    : mStorage{InValid}
{
}

File::File(QString filePath)
    : mPlace()
    , mFilePath(std::move(filePath))
    , mStorage{Local}
{
}

File::File(git_repository *repo, git_tree_entry *entry)
    : mRepo{repo}
    , mEntry{entry}
    , mStorage{Entry}
{
}

File::~File()
{
    if (mEntry)
        git_tree_entry_free(mEntry);
}

File::File(Manager *git, QString place, QString filePath)
    : mPlace(std::move(place))
    , mFilePath(std::move(filePath))
    , mGit(git)
    , mStorage{Git}
{
    Q_ASSERT(mGit);

    if (mFilePath.startsWith("/"))
        mFilePath = mFilePath.mid(1);
}

File::File(const File &other) = default;

// File::File(File &&other)
//     : _place(std::move(other._place)), _filePath(std::move(other._filePath)),
//       _git(std::move(other._git)), _storage(std::move(other._storage))
//{

//}

File &File::operator=(const File &other) = default;

const QString &File::place() const
{
    return mPlace;
}

void File::setPlace(const QString &newPlace)
{
    mPlace = newPlace;
}

QString File::fileName() const
{
    switch (mStorage) {
    case Entry: {
        QString name = git_tree_entry_name(mEntry);
        return name;
    }
    case Git:
        return mFilePath;
    case Local:
    case InValid:
        break;
    }
    return mFilePath;
}

void File::setFileName(const QString &newFileName)
{
    mFilePath = newFileName;
}

Manager *File::git() const
{
    return mGit;
}

QString File::displayName() const
{
    switch (mStorage) {
    case InValid:
        return {};
    case Local:
        return mFilePath;
    case Git:
        return mPlace + QLatin1Char(':') + mFilePath;
    case Entry: {
        QString name = git_tree_entry_name(mEntry);
        return name;
    }
    }

    return {};
}

bool File::save(const QString &path) const
{
    QFile f{path};
    if (!f.open(QIODevice::WriteOnly))
        return false;
    const auto buffer = content(); // mGit->runGit({QStringLiteral("show"), mPlace + QLatin1Char(':') + mFilePath});
    f.write(buffer.toUtf8());
    f.close();
    return true;
}

QString File::saveAsTemp() const
{
    QFileInfo fi{mFilePath};
    QString fileName;
    const auto tempLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (tempLocation.isEmpty())
        return {};

    fileName = QStringLiteral("%1/%2-%3.%4").arg(tempLocation, fi.baseName(), QUuid::createUuid().toString(QUuid::Id128), fi.suffix());

    if (save(fileName))
        return fileName;
    return {};
}

QString File::content() const
{
    switch (mStorage) {
    case InValid:
        return {};
    case Local: {
        QFile f(mFilePath);
        if (!f.open(QIODevice::ReadOnly))
            return {};
        return f.readAll();
    }
    case Git:
    case Entry:
        return stringContent(); // mGit->fileContent(mPlace, mFilePath); // mGit->runGit({QStringLiteral("show"), mPlace + QLatin1Char(':') + mFilePath});
    }

    return {};
}

QString File::stringContent() const
{
    git_object *placeObject{nullptr};
    git_commit *commit{nullptr};
    git_tree *tree{nullptr};
    git_tree_entry *entry{nullptr};
    git_blob *blob{nullptr};

    BEGIN
    if (mEntry) {
        STEP git_blob_lookup(&blob, mRepo, git_tree_entry_id(mEntry));
    } else {
        STEP git_revparse_single(&placeObject, mGit->mRepo, toConstChars(mPlace));
        STEP git_commit_lookup(&commit, mGit->mRepo, git_object_id(placeObject));
        STEP git_commit_tree(&tree, commit);

        STEP git_tree_entry_bypath(&entry, tree, toConstChars(mFilePath));
        STEP git_blob_lookup(&blob, mGit->mRepo, git_tree_entry_id(entry));
    }

    if (err)
        return {};

    QString ch = (char *)git_blob_rawcontent(blob);

    git_object_free(placeObject);
    git_commit_free(commit);
    git_blob_free(blob);
    git_tree_entry_free(entry);
    git_tree_free(tree);

    return ch;
}

} // namespace Git
