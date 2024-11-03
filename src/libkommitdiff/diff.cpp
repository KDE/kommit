/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diff.h"

#include <QDir>

namespace Diff
{

bool isFilesSame(const QString &file1, const QString &file2)
{
    QFileInfo fi1{file1};
    QFileInfo fi2{file2};

    if (fi1.size() != fi2.size() /* || fi1.lastModified() != fi2.lastModified()*/)
        return false;

    QFile f1{file1};
    QFile f2{file2};

    if (!f1.open(QIODevice::ReadOnly) || !f2.open(QIODevice::ReadOnly))
        return false;

    while (!f1.atEnd() && !f2.atEnd()) {
        auto buffer1 = f1.read(100);
        auto buffer2 = f2.read(100);
        if (buffer1 != buffer2) {
            f1.close();
            f2.close();
            return false;
        }
    }

    f1.close();
    f2.close();

    return true;
}
void browseDir(QStringList &filesList, const QString &dirPath, const QString &basePath)
{
    QDir d(basePath + QLatin1Char('/') + dirPath);
    const auto files = d.entryList(QDir::NoDotAndDotDot | QDir::Files);
    for (const auto &f : files) {
        if (dirPath.isEmpty())
            filesList.append(dirPath + f);
        else
            filesList.append(dirPath + QLatin1Char('/') + f);
    }

    const auto dirs = d.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    for (const auto &dir : dirs) {
        if (dirPath.isEmpty())
            browseDir(filesList, dirPath + dir, basePath);
        else
            browseDir(filesList, dirPath + QLatin1Char('/') + dir, basePath);
    }
}
QMap<QString, DiffType> diffDirs(const QString &dir1, const QString &dir2)
{
    auto d1 = dir1;
    auto d2 = dir2;
    QMap<QString, DiffType> map;

    if (!d1.endsWith(QLatin1Char('/')))
        d1.append(QLatin1Char('/'));

    if (!d2.endsWith(QLatin1Char('/')))
        d2.append(QLatin1Char('/'));

    QStringList files1, files2;
    browseDir(files1, {}, d1);
    browseDir(files2, {}, d2);

    for (const auto &file : std::as_const(files1)) {
        if (!files2.contains(file)) {
            map.insert(file, DiffType::Removed);
        } else {
            map.insert(file, isFilesSame(d1 + file, d2 + file) ? DiffType::Unchanged : DiffType::Modified);
            files2.removeOne(file);
        }
    }

    for (const auto &file : std::as_const(files2))
        map.insert(file, DiffType::Added);

    return map;
}

}
