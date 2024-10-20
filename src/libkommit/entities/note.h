/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QSharedPointer>
#include <QString>

#include <git2/types.h>

#include <Kommit/IOid>
#include <Kommit/Oid>

namespace Git
{

class Signature;

class NotePrivate;
class LIBKOMMIT_EXPORT Note : public IOid
{
public:
    explicit Note(git_note *note = nullptr);
    Note(const Note &other);

    Note &operator=(const Note &other);
    bool operator==(const Note &other) const;
    bool operator!=(const Note &other) const;

    [[nodiscard]] Signature author() const;
    [[nodiscard]] Signature committer() const;
    [[nodiscard]] QString message() const;
    Oid oid() const override;

    [[nodiscard]] bool isNull() const;

private:
    QSharedPointer<NotePrivate> d;
};
}
