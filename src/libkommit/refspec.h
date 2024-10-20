/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/remote.h>

#include <QSharedPointer>

namespace Git
{

class RefSpecPrivate;

class LIBKOMMIT_EXPORT RefSpec
{
public:
    enum class Direction { DirectionFetch = GIT_DIRECTION_FETCH, DirectionPush = GIT_DIRECTION_PUSH };

    explicit RefSpec(const git_refspec *refspecs = nullptr);

    RefSpec(const RefSpec &other);
    RefSpec &operator=(const RefSpec &other);
    bool operator==(const RefSpec &other) const;
    bool operator!=(const RefSpec &other) const;

    [[nodiscard]] QString name() const;
    [[nodiscard]] Direction direction() const;
    [[nodiscard]] QString destionation() const;
    [[nodiscard]] QString source() const;
    [[nodiscard]] bool isNull() const;

private:
    QSharedPointer<RefSpecPrivate> d;
};
}
