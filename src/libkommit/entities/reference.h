/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include "object.h"

#include <git2/types.h>

#include <QSharedPointer>
#include <QString>

namespace Git
{

class Note;
class Branch;
class Tag;
class Remote;
class Oid;

class LIBKOMMIT_EXPORT Reference
{
public:
    Reference();
    Reference(git_reference *ref);
    ~Reference();

    enum class Type {
        Invalid = GIT_REFERENCE_INVALID,
        Direct = GIT_REFERENCE_DIRECT,
        Symbolic = GIT_REFERENCE_SYMBOLIC,
        All = GIT_REFERENCE_ALL
    };

    [[nodiscard]] bool isNote() const;
    [[nodiscard]] bool isBranch() const;
    [[nodiscard]] bool isTag() const;
    [[nodiscard]] bool isRemote() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString shorthand() const;
    [[nodiscard]] Type type() const;
    [[nodiscard]] QSharedPointer<Oid> target() const;

    QSharedPointer<Object> peel(Object::Type type) const;

    [[nodiscard]] QSharedPointer<Note> toNote() const;
    [[nodiscard]] QSharedPointer<Branch> toBranch() const;
    [[nodiscard]] QSharedPointer<Tag> toTag() const;
    [[nodiscard]] QSharedPointer<Remote> toRemote() const;

    [[nodiscard]] static bool isValidName(const QString &name);

    [[nodiscard]] git_reference *refPtr() const;

private:
    git_reference *ptr{nullptr};
};

}
