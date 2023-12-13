/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kmessageboxhelper.h"

#include <KMessageBox>
#include <KStandardGuiItem>
#include <kwidgetsaddons_version.h>

KMessageBoxHelper::KMessageBoxHelper()
{
}

bool KMessageBoxHelper::removeQuestion(QWidget *parent, const QString &text, const QString &caption)
{
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r = KMessageBox::questionTwoActions(parent, text, caption, KStandardGuiItem::remove(), KStandardGuiItem::cancel());
    return r == KMessageBox::ButtonCode::PrimaryAction;
#else
    auto r = KMessageBox::questionYesNo(parent, text, caption);
    return r == KMessageBox::Yes;
#endif
}

bool KMessageBoxHelper::applyQuestion(QWidget *parent, const QString &text, const QString &caption)
{
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r = KMessageBox::questionTwoActions(parent, text, caption, KStandardGuiItem::apply(), KStandardGuiItem::cancel());
    return r == KMessageBox::ButtonCode::PrimaryAction;
#else
    auto r = KMessageBox::questionYesNo(parent, text, caption);
    return r == KMessageBox::Yes;
#endif
}
