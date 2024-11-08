/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>

#include <git2/rebase.h>

namespace Git
{

class RebaseOptionsPrivate;
class RebaseOptions
{
    Q_GADGET

public:
    RebaseOptions();
    ~RebaseOptions();

    void apply(git_rebase_options *opts);

private:
    QScopedPointer<RebaseOptionsPrivate> d;
};

}
