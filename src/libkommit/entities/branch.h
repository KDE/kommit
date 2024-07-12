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
class Reference;

class BranchPrivate;
class LIBKOMMIT_EXPORT Branch : public ITree
{
public:
    explicit Branch(git_reference *branch);
    ~Branch();

    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString refName() const;
    Q_REQUIRED_RESULT QString upStreamName() const;
    Q_REQUIRED_RESULT QString remoteName() const;

    Q_REQUIRED_RESULT bool isHead() const;

    Q_REQUIRED_RESULT QSharedPointer<Tree> tree() const override;
    Q_REQUIRED_RESULT QSharedPointer<Commit> commit();
    Q_REQUIRED_RESULT QSharedPointer<Reference> reference() const;

    Q_REQUIRED_RESULT QString treeTitle() const override;
    git_reference *refPtr() const;

private:
    BranchPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Branch)
};

}
