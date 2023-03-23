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
    mGit->setConfig(QStringLiteral("alias.klient"), QStringLiteral("!kommit"), Git::Manager::ConfigGlobal);
    mGit->setConfig(QStringLiteral("alias.gkdiff"), QStringLiteral("'difftool --dir-diff --tool=kommitdiff'"), Git::Manager::ConfigGlobal);

    mGit->setConfig(QStringLiteral("mergetool.kommitmerge.cmd"),
                    QStringLiteral(R"(kommitmerge "$BASE" "$LOCAL" "$REMOTE" "$MERGED")"),
                    Git::Manager::ConfigGlobal);
    mGit->setConfig(QStringLiteral("mergetool.kommitmerge.trustExitCode"), QStringLiteral("true"), Git::Manager::ConfigGlobal);

    mGit->setConfig(QStringLiteral("difftool.kommitdiff.cmd"), QStringLiteral(R"(kommitdiff "$LOCAL" "$REMOTE")"), Git::Manager::ConfigGlobal);
}
