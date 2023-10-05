/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"
#include "signature.h"

#include <QString>

#include <git2/types.h>

namespace Git
{

class LIBKOMMIT_EXPORT Note
{
public:
    Note(git_note *note);

    Q_REQUIRED_RESULT Signature author() const;
    Q_REQUIRED_RESULT Signature committer() const;
    Q_REQUIRED_RESULT QString mesage() const;

private:
    git_note *mNote;
    Signature mAuthor;
    Signature mCommitter;
    QString mMesage;
};

}
