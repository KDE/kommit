/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "interfaces.h"
#include "libkommit_export.h"

#include <QSharedPointer>
#include <QString>

#include <git2/types.h>

namespace Git
{

class Signature;

class LIBKOMMIT_EXPORT Note : public IOid
{
public:
    explicit Note(git_note *note);

    Q_REQUIRED_RESULT QSharedPointer<Signature> author() const;
    Q_REQUIRED_RESULT QSharedPointer<Signature> committer() const;
    Q_REQUIRED_RESULT QString mesage() const;
    QSharedPointer<Oid> oid() const override;

private:
    git_note *const mNote;
    QSharedPointer<Signature> mAuthor;
    QSharedPointer<Signature> mCommitter;
    QString mMesage;
};
}
