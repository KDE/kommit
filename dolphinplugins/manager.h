/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>
#include <filestatus.h>

namespace Git
{

class MiniManager
{
public:
    explicit MiniManager(const QString &path);

    Q_REQUIRED_RESULT const QString &path() const;
    void setPath(const QString &newPath);

    Q_REQUIRED_RESULT bool isValid() const;
    Q_REQUIRED_RESULT QList<FileStatus> repoFilesStatus() const;

private:
    QString mPath;
    bool mIsValid{false};
};

} // namespace Git
