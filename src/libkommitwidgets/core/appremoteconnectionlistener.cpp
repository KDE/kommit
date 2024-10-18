/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "appremoteconnectionlistener.h"

#include "dialogs/certificateinfodialog.h"
#include "dialogs/credentialdialog.h"

#include <Kommit/Certificate>
#include <Kommit/Credential>

AppRemoteConnectionListener *AppRemoteConnectionListener::instance()
{
    static AppRemoteConnectionListener instance;
    return &instance;
}

void AppRemoteConnectionListener::credentialRequeted(const QString &url, Git::Credential *cred, bool *accept)
{
    CredentialDialog d{mParentWidget};
    d.setUrl(url);

    if (d.exec() == QDialog::Accepted) {
        cred->setUsername(d.username());
        cred->setPassword(d.password());
        *accept = true;
    } else {
        *accept = false;
    }
}

void AppRemoteConnectionListener::certificateCheck(Git::Certificate *cert, bool *accept)
{
    if (++mRetryCount > 3) {
        *accept = false;
        return;
    }
    CertificateInfoDialog d{cert, mParentWidget};
    *accept = d.exec() == QDialog::Accepted;
}

QWidget *AppRemoteConnectionListener::parentWidget() const
{
    return mParentWidget;
}

void AppRemoteConnectionListener::setParentWidget(QWidget *parentWidget)
{
    mParentWidget = parentWidget;
    mRetryCount = 0;
}
