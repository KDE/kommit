/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>

namespace Git {

class Submodule
{
public:
    Submodule();

    const QString &path() const;
    void setPath(const QString &newPath);
    const QString &commitHash() const;
    void setCommitHash(const QString &newCommitHash);
    const QString &refName() const;
    void setRefName(const QString &newRefName);

private:
    QString _path;
    QString _commitHash;
    QString _refName;
};

} // namespace Git

