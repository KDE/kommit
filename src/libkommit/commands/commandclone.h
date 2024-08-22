/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libkommit_export.h"
namespace Git
{

class LIBKOMMIT_EXPORT CloneCommand : public AbstractCommand
{
    Q_OBJECT

public:
    explicit CloneCommand(QObject *parent = nullptr);

    [[nodiscard]] const QString &repoUrl() const;
    void setRepoUrl(const QString &newRepoUrl);
    [[nodiscard]] const QString &localPath() const;
    void setLocalPath(const QString &newLocalPath);
    [[nodiscard]] const QString &branch() const;
    void setBranch(const QString &newBranch);
    [[nodiscard]] int depth() const;
    void setDepth(int newDepth);
    [[nodiscard]] const QString &origin() const;
    void setOrigin(const QString &newOrigin);
    [[nodiscard]] bool recursive() const;
    void setRecursive(bool newRecursive);

    [[nodiscard]] QStringList generateArgs() const override;
    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;
    [[nodiscard]] bool supportProgress() const override;

private:
    QString mRepoUrl;
    QString mLocalPath;
    QString mBranch;
    QString mOrigin;
    int mDepth{-1};
    bool mRecursive{false};
};

} // namespace Git
