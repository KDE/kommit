/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "appconfig.h"
#include "git/gitmanager.h"

AppConfig::AppConfig(Git::Manager *git)
    : mGit(git)
{
}

void AppConfig::apply()
{
    mGit->setConfig("alias.klient", "!gitklient", Git::Manager::ConfigGlobal);
    mGit->setConfig("alias.gkdiff", "'difftool --dir-diff --tool=gitklientdiff'", Git::Manager::ConfigGlobal);

    mGit->setConfig("mergetool.gitklientmerge.cmd", R"(gitklientmerge "$BASE" "$LOCAL" "$REMOTE" "$MERGED")", Git::Manager::ConfigGlobal);
    mGit->setConfig("mergetool.gitklientmerge.trustExitCode", "true", Git::Manager::ConfigGlobal);

    mGit->setConfig("difftool.gitklientdiff.cmd", R"(gitklientdiff "$LOCAL" "$REMOTE")", Git::Manager::ConfigGlobal);
}
