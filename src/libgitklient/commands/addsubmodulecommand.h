/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractcommand.h"
#include "libgitklient_export.h"

namespace Git
{

class LIBGITKLIENT_EXPORT AddSubmoduleCommand : public AbstractCommand
{
    Q_OBJECT

public:
    explicit AddSubmoduleCommand(Manager *git);

    QStringList generateArgs() const override;

    bool force() const;
    void setForce(bool newForce);

    QString branch() const;
    void setbranch(QString newbranch);

    const QString &url() const;
    void setUrl(const QString &newUrl);

    const QString &localPath() const;
    void setLocalPath(const QString &newLocalPath);

private:
    bool mForce{false};
    QString mBranch;
    QString mUrl;
    QString mLocalPath;
};

}
