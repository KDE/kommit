/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>
#include <filestatus.h>

namespace Git {

class MiniManager
{
public:
    explicit MiniManager(const QString &path);

    const QString &path() const;
    void setPath(const QString &newPath);

    bool isValid() const;
    QList<FileStatus> repoFilesStatus() const;

private:
    QString mPath;
    bool mIsValid{false};
};

} // namespace Git

