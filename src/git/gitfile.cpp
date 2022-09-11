/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gitfile.h"
#include "gitmanager.h"

#include <QFile>
#include <utility>

namespace Git
{

File::StorageType File::storage() const
{
    return _storage;
}

File::File()
    : _storage{InValid}
{
}

File::File(QString filePath)
    : _place()
    , _filePath(std::move(filePath))
    , _storage{Local}
{
}

File::File(QString place, QString filePath, Manager *git)
    : _place(std::move(place))
    , _filePath(std::move(filePath))
    , _git(git)
    , _storage{Git}
{
    if (!_git)
        _git = Manager::instance();
}

File::File(const File &other)

    = default;

// File::File(File &&other)
//     : _place(std::move(other._place)), _filePath(std::move(other._filePath)),
//       _git(std::move(other._git)), _storage(std::move(other._storage))
//{

//}

File &File::operator=(const File &other) = default;

const QString &File::place() const
{
    return _place;
}

void File::setPlace(const QString &newPlace)
{
    _place = newPlace;
}

const QString &File::fileName() const
{
    return _filePath;
}

void File::setFileName(const QString &newFileName)
{
    _filePath = newFileName;
}

Manager *File::git() const
{
    return _git;
}

void File::setGit(Manager *newGit)
{
    _git = newGit;
}

QString File::displayName() const
{
    switch (_storage) {
    case InValid:
        return {};
    case Local:
        return _filePath;
    case Git:
        return _place + ":" + _filePath;
    }

    return {};
}

bool File::save(const QString &path) const
{
    QFile f{path};
    if (!f.open(QIODevice::WriteOnly))
        return false;
    auto buffer = _git->runGit({"show", _place + ":" + _filePath});
    f.write(buffer);
    f.close();
    return true;
}

QString File::content() const
{
    switch (_storage) {
    case InValid:
        return {};
    case Local: {
        QFile f(_filePath);
        if (!f.open(QIODevice::ReadOnly))
            return {};
        return f.readAll();
    }
    case Git:
        return _git->runGit({"show", _place + ":" + _filePath});
    }

    return {};
}

} // namespace Git
