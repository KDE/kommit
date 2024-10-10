/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractcache.h"

#include <repository.h>

namespace Git
{
namespace Impl
{

git_repository *getRepo(Repository *manager)
{
    return manager->repoPtr();
}

}
}
