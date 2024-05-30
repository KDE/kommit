/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

namespace Git
{
class Manager;
}
class AppConfig
{
public:
    explicit AppConfig(Git::Manager *git);
    void apply();

private:
    Git::Manager *mGit = nullptr;
};
