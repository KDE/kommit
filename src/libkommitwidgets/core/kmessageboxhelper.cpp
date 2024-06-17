/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kmessageboxhelper.h"

#ifdef LIBKOMMIT_WIDGET_USE_KF
#include <KMessageBox>
#include <KStandardGuiItem>
#include <kwidgetsaddons_version.h>
#else
#include <QMessageBox>
#endif

KMessageBoxHelper::KMessageBoxHelper()
{
}

bool KMessageBoxHelper::removeQuestion(QWidget *parent, const QString &text, const QString &caption)
{
#ifdef LIBKOMMIT_WIDGET_USE_KF
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r = KMessageBox::questionTwoActions(parent, text, caption, KStandardGuiItem::remove(), KStandardGuiItem::cancel());
    return r == KMessageBox::ButtonCode::PrimaryAction;
#else
    auto r = KMessageBox::questionYesNo(parent, text, caption);
    return r == KMessageBox::Yes;
#endif

#else
    return QMessageBox::Yes == QMessageBox::question(parent, caption, text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
#endif
}

bool KMessageBoxHelper::applyQuestion(QWidget *parent, const QString &text, const QString &caption)
{
#ifdef LIBKOMMIT_WIDGET_USE_KF
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r = KMessageBox::questionTwoActions(parent, text, caption, KStandardGuiItem::apply(), KStandardGuiItem::cancel());
    return r == KMessageBox::ButtonCode::PrimaryAction;
#else
    auto r = KMessageBox::questionYesNo(parent, text, caption);
    return r == KMessageBox::Yes;
#endif
#else
    return QMessageBox::Yes == QMessageBox::question(parent, caption, text, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
#endif
}

void KMessageBoxHelper::error(QWidget *parent, const QString &text, const QString &title)
{
#ifdef LIBKOMMIT_WIDGET_USE_KF
    KMessageBox::error(parent, text, title);
#else
    QMessageBox::critical(parent, text, title);
#endif
}

void KMessageBoxHelper::information(QWidget *parent, const QString &text, const QString &title)
{
#ifdef LIBKOMMIT_WIDGET_USE_KF
    KMessageBox::information(parent, text, title);
#else
    QMessageBox::information(parent, text, title);
#endif
}
