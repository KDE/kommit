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
public:
    enum StorageType { InValid, Git, Local };
    File();
    explicit File(QString filePath);
    File(QString place, QString filePath, Manager *git = nullptr);
    File(const File &other);
    //    File(File &&other);

    File &operator=(const File &other);

    bool save(const QString &path) const;
    Q_REQUIRED_RESULT QString content() const;
    Q_REQUIRED_RESULT const QString &place() const;
    void setPlace(const QString &newPlace);
    Q_REQUIRED_RESULT const QString &fileName() const;
    void setFileName(const QString &newFileName);
    Manager *git() const;
    void setGit(Manager *newGit);

    Q_REQUIRED_RESULT QString displayName() const;
    Q_REQUIRED_RESULT StorageType storage() const;

private:
    QString mPlace;
    QString mFilePath;
    Manager *mGit = nullptr;

    StorageType mStorage;
};

} // namespace Git
