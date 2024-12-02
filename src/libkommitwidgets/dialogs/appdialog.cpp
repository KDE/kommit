/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "appdialog.h"

#include <Kommit/Repository>

#include "certificateinfodialog.h"
#include "credentialdialog.h"

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

void AppDialog::slotCredentialRequested(const QString &url, Git::Credential *cred, bool *accept)
{
    CredentialDialog d{this};
    d.setUrl(url);

    if (d.exec() == QDialog::Accepted) {
        cred->setUsername(d.username());
        cred->setPassword(d.password());
        *accept = true;
    } else {
        *accept = false;
    }
}

void AppDialog::slotCertificateCheck(const Git::Certificate &cert, bool *accept)
{
    if (cert.isValid()) {
        *accept = true;
        return;
    }
    if (++mRetryCount > 3) {
        *accept = false;
        return;
    }
    CertificateInfoDialog d{cert, this};
    *accept = d.exec() == QDialog::Accepted;
}

void AppDialog::connectRemoteCallbacks(const Git::RemoteCallbacks *callbacks)
{
    mRetryCount = 0;
    connect(callbacks, &Git::RemoteCallbacks::credentialRequested, this, &AppDialog::slotCredentialRequested);
    connect(callbacks, &Git::RemoteCallbacks::certificateCheck, this, &AppDialog::slotCertificateCheck);
}

#include "moc_appdialog.cpp"
