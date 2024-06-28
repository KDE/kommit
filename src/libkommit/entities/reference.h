/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/types.h>

#include <QSharedPointer>
#include <QString>

namespace Git
{

class Note;
class Branch;
class Tag;
class Remote;

class LIBKOMMIT_EXPORT Reference
{
public:
    Reference();
    Reference(git_reference *ref);
    ~Reference();

    [[nodiscard]] bool isNote() const;
    [[nodiscard]] bool isBranch() const;
    [[nodiscard]] bool isTag() const;
    [[nodiscard]] bool isRemote() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString shorthand() const;

    [[nodiscard]] QSharedPointer<Note> toNote() const;
    [[nodiscard]] QSharedPointer<Branch> toBranch() const;
    [[nodiscard]] QSharedPointer<Tag> toTag() const;
    [[nodiscard]] QSharedPointer<Remote> toRemote() const;

private:
    git_reference *ptr{nullptr};
    bool mIsNote{false};
    bool mIsBranch{false};
    bool mIsTag{false};
    bool mIsRemote{false};

    QString mName;
    QString mShorthand;
};

}
