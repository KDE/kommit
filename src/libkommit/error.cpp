/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "error.h"

#include <git2/errors.h>

namespace Git
{

Error::Error()
{
    auto e = git_error_last();
}

int Error::klass()
{
    return git_error_last()->klass;
}

QString Error::message()
{
    return QString{git_error_last()->message};
}

}
