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

class ReferencePrivate;
class LIBKOMMIT_EXPORT Reference
{
public:
    Reference();
    explicit Reference(git_reference *ref);

    Reference(const Reference &other);
    Reference &operator=(const Reference &other);
    bool operator==(const Reference &other) const;
    bool operator!=(const Reference &other) const;

    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_reference *data() const;
    [[nodiscard]] const git_reference *constData() const;

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
    [[nodiscard]] Oid target() const;

    [[nodiscard]] Object peel(Object::Type type) const;

    [[nodiscard]] Note toNote() const;
    [[nodiscard]] Branch toBranch() const;
    [[nodiscard]] Tag toTag() const;
    [[nodiscard]] Remote toRemote() const;

    [[nodiscard]] static bool isValidName(const QString &name);

    [[nodiscard]] git_reference *refPtr() const;

private:
    QSharedPointer<ReferencePrivate> d;
};
}
