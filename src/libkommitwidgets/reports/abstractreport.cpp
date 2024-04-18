/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractreport.h"
#include "gitmanager.h"

AbstractReport::AbstractReport(Git::Manager *git, QObject *parent)
    : QAbstractListModel{parent}
    , mGit{git}
{
}

AbstractReport::~AbstractReport()
{
}

#include "abstractreport.moc"
