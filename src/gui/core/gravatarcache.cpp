/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gravatarcache.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>

GravatarCache::GravatarCache(QObject *parent)
    : QObject{parent}
{
    const auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/avatars");
    QDir d;
    d.mkpath(path);
}

QString GravatarCache::avatarPath(const QString &email)
{
    const auto emailHash = QCryptographicHash::hash(email.toUtf8(), QCryptographicHash::Md5).toHex().toLower();

    if (mAvatarsCache.contains(emailHash))
        return mAvatarsCache[emailHash];

    const auto avatarUrl = QStringLiteral("https://www.gravatar.com/avatar/%1").arg(emailHash);

    QNetworkRequest request{QUrl{avatarUrl}};

    QNetworkReply *reply = mNet.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, emailHash, avatarUrl, reply]() {
        QFile avatarFile;
        if (avatarFile.open(QIODevice::WriteOnly)) {
            avatarFile.write(reply->readAll());
            avatarFile.close();
            mAvatarsCache.insert(emailHash, avatarUrl);
        }
        delete reply;
    });
    return {};
}

#include "moc_gravatarcache.cpp"
