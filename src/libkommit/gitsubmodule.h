/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libkommit_export.h"
#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT Submodule
{
public:
    Submodule();

    Q_REQUIRED_RESULT const QString &path() const;
    void setPath(const QString &newPath);
    Q_REQUIRED_RESULT const QString &commitHash() const;
    void setCommitHash(const QString &newCommitHash);
    Q_REQUIRED_RESULT const QString &refName() const;
    void setRefName(const QString &newRefName);

    Q_REQUIRED_RESULT const QString &url() const;
    void setUrl(const QString &newUrl);

private:
    QString mUrl;
    QString mPath;
    QString mCommitHash;
    QString mRefName;
};

} // namespace Git
