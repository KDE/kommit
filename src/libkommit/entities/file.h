/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include <QString>

#include <git2/types.h>

namespace Git
{

class Manager;
class LIBKOMMIT_EXPORT File
{
public:
    enum StorageType { InValid, Git, Local, Entry };
    File();
    explicit File(QString filePath);
    File(Manager *git, QString place, QString filePath);
    File(const File &other);
    File(git_repository *repo, git_tree_entry *entry);
    //    File(File &&other);
    ~File();

    File &operator=(const File &other);

    bool save(const QString &path) const;
    [[nodiscard]] QString saveAsTemp() const;

    [[nodiscard]] QString content() const;
    [[nodiscard]] const QString &place() const;
    void setPlace(const QString &newPlace);
    [[nodiscard]] QString fileName() const;
    void setFileName(const QString &newFileName);
    Manager *git() const;

    [[nodiscard]] QString displayName() const;
    [[nodiscard]] StorageType storage() const;

private:
    git_repository *mRepo{nullptr};
    git_tree_entry *mEntry{nullptr};

    QString mPlace;
    QString mFilePath;
    Manager *mGit = nullptr;

    StorageType mStorage;

    QString stringContent() const;
};

} // namespace Git
