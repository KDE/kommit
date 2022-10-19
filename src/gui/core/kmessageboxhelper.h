/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <QString>

#pragma once

class QWidget;

class KMessageBoxHelper
{
public:
    KMessageBoxHelper();
    static bool removeQuestion(QWidget *parent, const QString &text, const QString &caption = QString());
    static bool applyQuestion(QWidget *parent, const QString &text, const QString &caption = QString());
};
