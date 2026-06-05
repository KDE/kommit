/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kmessageboxhelper.h"

#include <KMessageBox>
#include <KStandardGuiItem>

KMessageBoxHelper::KMessageBoxHelper()
{
}

bool KMessageBoxHelper::removeQuestion(QWidget *parent, const QString &text, const QString &caption)
{
    auto r = KMessageBox::questionTwoActions(parent, text, caption, KStandardGuiItem::remove(), KStandardGuiItem::cancel());
    return r == KMessageBox::ButtonCode::PrimaryAction;
}

bool KMessageBoxHelper::applyQuestion(QWidget *parent, const QString &text, const QString &caption)
{
    auto r = KMessageBox::questionTwoActions(parent, text, caption, KStandardGuiItem::apply(), KStandardGuiItem::cancel());
    return r == KMessageBox::ButtonCode::PrimaryAction;
}
