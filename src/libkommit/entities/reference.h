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

    enum class Type { Invalid = GIT_REFERENCE_INVALID, Direct = GIT_REFERENCE_DIRECT, Symbolic = GIT_REFERENCE_SYMBOLIC, All = GIT_REFERENCE_ALL };

    Q_REQUIRED_RESULT bool isNote() const;
    Q_REQUIRED_RESULT bool isBranch() const;
    Q_REQUIRED_RESULT bool isTag() const;
    Q_REQUIRED_RESULT bool isRemote() const;
    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString shorthand() const;
    Q_REQUIRED_RESULT Type type() const;
    [[nodiscard]] QSharedPointer<Oid> target() const;

    QSharedPointer<Object> peel(Object::Type type) const;

    Q_REQUIRED_RESULT QSharedPointer<Note> toNote() const;
    Q_REQUIRED_RESULT QSharedPointer<Branch> toBranch() const;
    Q_REQUIRED_RESULT QSharedPointer<Tag> toTag() const;
    Q_REQUIRED_RESULT QSharedPointer<Remote> toRemote() const;

    Q_REQUIRED_RESULT static bool isValidName(const QString &name);

    Q_REQUIRED_RESULT git_reference *refPtr() const;

private:
    git_reference *ptr{nullptr};
};

}
