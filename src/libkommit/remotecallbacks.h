/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>

#include <git2/remote.h>

#include <Kommit/Certificate>
#include <Kommit/Credential>
#include <Kommit/Oid>
#include <Kommit/Reference>
#include <Kommit/Remote>

#include "libkommit_export.h"

namespace Git
{

struct FetchTransferStat;
struct PackProgress;
class Repository;

class LIBKOMMIT_EXPORT RemoteCallbacks : public QObject
{
    Q_OBJECT
public:
    explicit RemoteCallbacks(QObject *parent = nullptr);

    void apply(git_remote_callbacks *callbacks, Repository *repo);

Q_SIGNALS:
    void message(const QString &message);
    void transferProgress(const FetchTransferStat *stat);
    void packProgress(const PackProgress *p);
    void updateRef(const Reference &reference, const Oid &a, const Oid &b);
    void credentialRequested(const QString &url, Credential *cred, bool *accept);
    void certificateCheck(const Certificate &cert, bool *accept);
    void remoteReady(const Remote &remote, Remote::Direction direction, QString *newUrl);
};

} // namespace Git
