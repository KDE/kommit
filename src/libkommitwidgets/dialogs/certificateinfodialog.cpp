/*
SPDX-FileCopyrightText: 2023 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "certificateinfodialog.h"

#include <KLocalizedString>
#include <Kommit/Certificate>

CertificateInfoDialog::CertificateInfoDialog(const Git::Certificate &cert, QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    switch (cert.type()) {
    case Git::Certificate::Type::None:
        break;
    case Git::Certificate::Type::X509: {
        auto &data = cert.x509();

        labelHost->setText(cert.host());
        labelIsValid->setText(cert.isValid() ? i18n("Yes") : i18n("No"));

        labelSubject->setText(data.subject);
        labelIssuer->setText(data.issuer);
        labelSerialNumber->setText(data.serialNumber);
        labelValidFrom->setText(data.validFrom);
        labelValidUntil->setText(data.validUntil);
        labelPublicKeyType->setText(data.publicKeyType);
        labelPublicKeyBits->setText(data.publicKeyBits);
        labelSignatureAlgorithm->setText(data.signatureAlgorithm);
        break;
    }
    case Git::Certificate::Type::HostkeyLibssh2:
        break;
    case Git::Certificate::Type::Strarray:
        break;
    }
}

#include "moc_certificateinfodialog.cpp"
