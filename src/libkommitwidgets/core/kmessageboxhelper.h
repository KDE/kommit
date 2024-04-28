/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"
#include <QString>

class QWidget;

class LIBKOMMITWIDGETS_EXPORT KMessageBoxHelper
{
public:
    KMessageBoxHelper();
    static bool removeQuestion(QWidget *parent, const QString &text, const QString &caption = QString());
    static bool applyQuestion(QWidget *parent, const QString &text, const QString &caption = QString());
    static void error(QWidget *parent, const QString &text, const QString &title = QString());
    static void information(QWidget *parent, const QString &text, const QString &title = QString());
};
