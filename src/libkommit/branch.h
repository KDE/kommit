/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>

#include "libkommit_export.h"

#include "git2/types.h"

namespace Git
{

class Note;
class LIBKOMMIT_EXPORT Branch
{
public:
    Branch(git_reference *branch);
    ~Branch();

    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString refName() const;
    Q_REQUIRED_RESULT QString upStreamName() const;
    Q_REQUIRED_RESULT QString remoteName() const;

    Note *note() const;

    Q_REQUIRED_RESULT bool isHead() const;

private:
    git_reference *mBranch;
    QString mName;
    QString mRefName;
    QString mUpStreamName;
    QString mRemoteName;
    bool mIsHead{false};
};

}
