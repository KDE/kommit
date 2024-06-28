/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>

#include "interfaces.h"
#include "libkommit_export.h"

#include "git2/types.h"

namespace Git
{

class Tree;
class Commit;

class LIBKOMMIT_EXPORT Branch : public ITree
{
public:
    explicit Branch(git_reference *branch);
    ~Branch();

    [[nodiscard]] QString name() const;
    [[nodiscard]] QString refName() const;
    [[nodiscard]] QString upStreamName() const;
    [[nodiscard]] QString remoteName() const;

    [[nodiscard]] bool isHead() const;

    [[nodiscard]] QSharedPointer<Tree> tree() const override;
    [[nodiscard]] QSharedPointer<Commit> commit() const;

    [[nodiscard]] QString treeTitle() const override;

private:
    git_reference *const mBranch;
    QString mName;
    QString mRefName;
    QString mUpStreamName;
    QString mRemoteName;
    bool mIsHead{false};
};

}
