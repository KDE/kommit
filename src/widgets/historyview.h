/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QTreeView>

class HistoryView : public QTreeView
{
    Q_OBJECT

public:
    explicit HistoryView(QWidget *parent = nullptr);
};

#endif // HISTORYVIEW_H
