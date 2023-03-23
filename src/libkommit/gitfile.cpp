/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gitfile.h"
#include "gitmanager.h"

#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUuid>
#include <utility>

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

File::File(Manager *git, QString place, QString filePath)
    : mPlace(std::move(place))
    , mFilePath(std::move(filePath))
    , mGit(git)
    , mStorage{Git}
{
    Q_ASSERT(mGit);
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

const QString &File::fileName() const
{
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

void File::setGit(Manager *newGit)
{
    mGit = newGit;
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
    }

    return {};
}

bool File::save(const QString &path) const
{
    QFile f{path};
    if (!f.open(QIODevice::WriteOnly))
        return false;
    const auto buffer = mGit->runGit({QStringLiteral("show"), mPlace + QLatin1Char(':') + mFilePath});
    f.write(buffer);
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
        return mGit->runGit({QStringLiteral("show"), mPlace + QLatin1Char(':') + mFilePath});
    }

    return {};
}

} // namespace Git
