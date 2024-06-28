/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include "signature.h"

#include <QSharedPointer>
#include <QString>

#include <git2/types.h>

namespace Git
{

class LIBKOMMIT_EXPORT Note
{
public:
    explicit Note(git_note *note);

    [[nodiscard]] QSharedPointer<Signature> author() const;
    [[nodiscard]] QSharedPointer<Signature> committer() const;
    [[nodiscard]] QString mesage() const;

private:
    git_note *const mNote;
    QSharedPointer<Signature> mAuthor;
    QSharedPointer<Signature> mCommitter;
    QString mMesage;
};

}
