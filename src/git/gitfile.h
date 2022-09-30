/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libgitklient_export.h"
#include <QString>

namespace Git
{

class Manager;
class LIBGITKLIENT_EXPORT File
{
private:
    QString mPlace;
    QString mFilePath;
    Manager *mGit = nullptr;

    enum StorageType { InValid, Git, Local };
    StorageType mStorage;

public:
    File();
    explicit File(QString filePath);
    File(QString place, QString filePath, Manager *git = nullptr);
    File(const File &other);
    //    File(File &&other);

    File &operator=(const File &other);

    bool save(const QString &path) const;
    QString content() const;
    const QString &place() const;
    void setPlace(const QString &newPlace);
    const QString &fileName() const;
    void setFileName(const QString &newFileName);
    Manager *git() const;
    void setGit(Manager *newGit);

    QString displayName() const;
    StorageType storage() const;
};

} // namespace Git
