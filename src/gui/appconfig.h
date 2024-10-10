/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

namespace Git
{
class Repository;
}
class AppConfig
{
public:
    explicit AppConfig(Git::Repository *git);
    void apply();

private:
    Git::Repository *const mGit;
};
