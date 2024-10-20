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

class TagPrivate;
class LIBKOMMIT_EXPORT Tag : public IOid
{
public:
    enum class Type { RegularTag, LightTag };

    Tag();
    explicit Tag(git_tag *tag);
    Tag(git_commit *commit, const QString &name);
    explicit Tag(Commit *parentCommit);

    Tag(const Tag &other);
    Tag &operator=(const Tag &other);
    bool operator==(const Tag &other) const;
    bool operator!=(const Tag &other) const;

    [[nodiscard]] git_tag *data() const;
    [[nodiscard]] const git_tag *constData() const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] const QString &name() const;
    [[nodiscard]] const QString &message() const;
    [[nodiscard]] QDateTime createTime() const;
    [[nodiscard]] const Signature &tagger() const;

    Commit commit() const;

    [[nodiscard]] Type type() const;
    Oid oid() const override;
    Object target() const;

private:
    QSharedPointer<TagPrivate> d;
};

} // namespace Git
