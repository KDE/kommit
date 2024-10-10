/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "appdialog.h"

#include "repository.h"
#include <QDesktopServices>
#include <QEvent>
#include <QWhatsThisClickedEvent>

AppDialog::AppDialog(QWidget *parent)
    : QDialog(parent)
    , mGit(Git::Repository::instance())
{
}

AppDialog::AppDialog(Git::Repository *git, QWidget *parent)
    : QDialog(parent)
    , mGit(git)
{
}

bool AppDialog::event(QEvent *event)
{
    if (event->type() == QEvent::WhatsThisClicked) {
        event->accept();
        auto whatsThisEvent = static_cast<QWhatsThisClickedEvent *>(event);
        QDesktopServices::openUrl(QUrl(whatsThisEvent->href()));
        return true;
    }
    return QDialog::event(event);
}

#include "moc_appdialog.cpp"
