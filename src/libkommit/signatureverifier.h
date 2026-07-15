/*
SPDX-FileCopyrightText: 2026 Li Bohai <lbhlbhlbh2002@icloud.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "entities/commitsignatureinfo.h"
#include "libkommit_export.h"

#include <QByteArray>
#include <QString>

#include <git2/types.h>

namespace Git
{

class LIBKOMMIT_EXPORT SignatureVerifier
{
public:
    SignatureVerifier();
    ~SignatureVerifier();

    SignatureVerifier(const SignatureVerifier &) = delete;
    SignatureVerifier &operator=(const SignatureVerifier &) = delete;

    CommitSignatureInfo verify(git_repository *repo, const git_oid *commitId);

private:
    CommitSignatureInfo verifyGpg(const QByteArray &signature, const QByteArray &signedData);
    CommitSignatureInfo verifySsh(const QByteArray &signature, const QByteArray &signedData);

    bool mGpgmeAvailable;
    bool mOpensslAvailable;
};

}
