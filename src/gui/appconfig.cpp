/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "appconfig.h"
#include "gitmanager.h"

AppConfig::AppConfig(Git::Manager *git)
    : mGit(git)
{
}

void AppConfig::apply()
{
    mGit->setConfig(QStringLiteral("alias.klient"), QStringLiteral("!gitklient"), Git::Manager::ConfigGlobal);
    mGit->setConfig(QStringLiteral("alias.gkdiff"), QStringLiteral("'difftool --dir-diff --tool=gitklientdiff'"), Git::Manager::ConfigGlobal);

    mGit->setConfig(QStringLiteral("mergetool.gitklientmerge.cmd"),
                    QStringLiteral(R"(gitklientmerge "$BASE" "$LOCAL" "$REMOTE" "$MERGED")"),
                    Git::Manager::ConfigGlobal);
    mGit->setConfig(QStringLiteral("mergetool.gitklientmerge.trustExitCode"), QStringLiteral("true"), Git::Manager::ConfigGlobal);

    mGit->setConfig(QStringLiteral("difftool.gitklientdiff.cmd"), QStringLiteral(R"(gitklientdiff "$LOCAL" "$REMOTE")"), Git::Manager::ConfigGlobal);
}
