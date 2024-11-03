/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>
#include <QString>

#include "libkommit_export.h"

#include "git2/types.h"

#include <Kommit/ITree>

namespace Git
{

class Tree;
class Commit;
class Reference;
class Object;

class BranchPrivate;
class LIBKOMMIT_EXPORT Branch : public ITree
{
public:
    Branch();
    explicit Branch(git_reference *branch);
    Branch(const Branch &other);
    Branch &operator=(const Branch &other);
    bool operator==(const Branch &other) const;
    bool operator!=(const Branch &other) const;
    bool operator<(const Branch &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_reference *data() const;

    [[nodiscard]] const git_reference *constData() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString refName() const;
    [[nodiscard]] QString upStreamName() const;
    [[nodiscard]] QString remoteName() const;

    [[nodiscard]] bool isHead() const;

    [[nodiscard]] Tree tree() const override;
    [[nodiscard]] Commit commit();
    [[nodiscard]] Reference reference() const;

    [[nodiscard]] QString treeTitle() const override;
    [[nodiscard]] Object object() const;

    git_reference *refPtr() const;

private:
    QSharedPointer<BranchPrivate> d;
};

}
