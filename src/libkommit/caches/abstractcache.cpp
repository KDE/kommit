/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractcache.h"

#include <gitmanager.h>

namespace Git
{
namespace Impl
{

git_repository *getRepo(Manager *manager)
{
    return manager->repoPtr();
}
};
};
