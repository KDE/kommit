/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractaction.h"

#include <QExplicitlySharedDataPointer>
#include <QString>

#include <Kommit/FileMode>

namespace Git
{

class Fetch;
class ClonePrivate;
class Repository;
class LIBKOMMIT_EXPORT Clone : public AbstractAction
{
public:
    Clone(Repository *repo, QObject *parent = nullptr);

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

protected:
    int exec() override;

private:
    QExplicitlySharedDataPointer<ClonePrivate> d;
};

}
