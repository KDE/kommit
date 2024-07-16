/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_changelogsdialog.h"

#include <QVersionNumber>

class ChangeLogsDialog : public QDialog, private Ui::ChangeLogsDialog
{
    Q_OBJECT

public:
    explicit ChangeLogsDialog(QWidget *parent = nullptr);

private:
    struct Row {
        QVersionNumber version;
        QStringList changes;
    };

    void slotTextBrowserAnchorClicked(const QUrl &arg1);
    QList<Row> data;

    void fillData();
    void showMarkdown();
};
