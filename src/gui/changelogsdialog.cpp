/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changelogsdialog.h"
#include "changelogsdata.h"

#include <KLocalizedString>

ChangeLogsDialog::ChangeLogsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    ChangeLogsData data;
    // s.append(QStringLiteral("\n\n<a href=\"to://show_more\">%1</a>").arg(i18n("Show more")));
    textBrowser->setMarkdown(data.generateMarkdown());
}

#include "moc_changelogsdialog.cpp"
