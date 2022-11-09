/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libgitklient_export.h"
namespace Git
{

class LIBGITKLIENT_EXPORT CloneCommand : public AbstractCommand
{
    Q_OBJECT

public:
    explicit CloneCommand(QObject *parent = nullptr);

    Q_REQUIRED_RESULT const QString &repoUrl() const;
    void setRepoUrl(const QString &newRepoUrl);
    Q_REQUIRED_RESULT const QString &localPath() const;
    void setLocalPath(const QString &newLocalPath);
    Q_REQUIRED_RESULT const QString &branch() const;
    void setBranch(const QString &newBranch);
    Q_REQUIRED_RESULT int depth() const;
    void setDepth(int newDepth);
    Q_REQUIRED_RESULT const QString &origin() const;
    void setOrigin(const QString &newOrigin);
    Q_REQUIRED_RESULT bool recursive() const;
    void setRecursive(bool newRecursive);

    Q_REQUIRED_RESULT QStringList generateArgs() const override;
    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;
    Q_REQUIRED_RESULT bool supportProgress() const override;

private:
    QString mRepoUrl;
    QString mLocalPath;
    QString mBranch;
    int mDepth{-1};
    QString mOrigin;
    bool mRecursive{false};
};

} // namespace Git
