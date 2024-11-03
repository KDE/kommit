/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>
#include <QExplicitlySharedDataPointer>

#include <Kommit/FileMode>

namespace Git
{

class Fetch;
class ClonePrivate;
class Repository;
class Clone
{
public:
    Clone(Repository *repo);

    void run();

    [[nodiscard]] QString url() const;
    void setUrl(const QString &url);

    [[nodiscard]] QString localPath() const;
    void setLocalPath(const QString &localPath);

    [[nodiscard]] FileMode fileMode() const;
    void setFileMode(const FileMode &fileMode);

    [[nodiscard]] FileMode dirMode() const;
    void setDirMode(const FileMode &dirMode);

    [[nodiscard]] int depth() const;
    void setDepth(const int &depth);

    [[nodiscard]] QString checkoutBranchName() const;
    void setCheckoutBranchName(const QString &checkoutBranchName);

    Fetch *fetch();

private:
    QExplicitlySharedDataPointer<ClonePrivate> d;
};

}
