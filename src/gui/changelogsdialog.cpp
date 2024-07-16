/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changelogsdialog.h"
#include "qdebug.h"

#include <KLocalizedString>

ChangeLogsDialog::ChangeLogsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    fillData();
    showMarkdown();
}

void ChangeLogsDialog::fillData()
{
    // clang-format off
    // Add change logs here

    data << Row{
        QVersionNumber{1, 6, 1},
        QStringList{
            i18n("Improve graph painting"),
            i18n("Improve on kommit lib (remove extra dependencies like kf)"),
            i18n("Add changes logs dialog"),
            i18n("Fix many small bugs")
        }
    };

    // clang-format on

    std::sort(data.begin(), data.end(), [](const Row &r1, const Row &r2) {
        return r1.version < r2.version;
    });
}

void ChangeLogsDialog::showMarkdown()
{
    QString s;
    for (const auto &row : std::as_const(data))
        s.append(QStringLiteral("## %1 %2 \n - %3\n").arg(i18n("Version"), row.version.toString(), row.changes.join(QStringLiteral("\n - "))));

    s.append(QStringLiteral("\n\n<a href=\"to://show_more\">%1</a>").arg(i18n("Show more")));
    textBrowser->setMarkdown(s);
}

void ChangeLogsDialog::slotTextBrowserAnchorClicked(const QUrl &arg1)
{
    qDebug() << arg1.toString();
}
