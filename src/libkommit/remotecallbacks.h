/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>

#include <git2/remote.h>

#include "libkommit_export.h"

namespace Git
{

class Reference;
class Oid;
class Certificate;
class Credential;
class FetchTransferStat;
class PackProgress;
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
    void updateRef(QSharedPointer<Reference> reference, QSharedPointer<Oid> a, QSharedPointer<Oid> b);
};

} // namespace Git
