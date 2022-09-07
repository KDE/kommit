/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef GITKLIENTCONFIG_H
#define GITKLIENTCONFIG_H

namespace Git {
class Manager;
}
class AppConfig
{
    Git::Manager *_git;
public:
    explicit AppConfig(Git::Manager *git);

    void apply();
};

#endif // GITKLIENTCONFIG_H
