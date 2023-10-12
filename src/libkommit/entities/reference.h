/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/types.h>

#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT Reference
{
public:
    Reference();
    Reference(git_reference *ref);
    ~Reference();

    Q_REQUIRED_RESULT bool isNote() const;
    Q_REQUIRED_RESULT bool isBranch() const;
    Q_REQUIRED_RESULT bool isTag() const;
    Q_REQUIRED_RESULT bool isRemote() const;
    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString shorthand() const;

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
