/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "entities/commit.h"
#include "entities/signature.h"
#include "libkommit_export.h"
#include <QDateTime>
#include <QSharedPointer>
#include <QString>

#include <git2/types.h>

namespace Git
{

class LIBKOMMIT_EXPORT Tag
{
public:
    Tag();
    Tag(git_tag *tag);

    Q_REQUIRED_RESULT const QString &name() const;
    void setName(const QString &newName);

    Q_REQUIRED_RESULT const QString &message() const;
    void setMessage(const QString &newMessage);

    Q_REQUIRED_RESULT QDateTime createTime() const;

    Q_REQUIRED_RESULT QSharedPointer<Signature> tagger() const;

    QSharedPointer<Commit> commit() const;

private:
    git_tag *mTagPtr{nullptr};
    QSharedPointer<Signature> mTagger;

    QString mName;
    QString mMessage;
};

} // namespace Git
