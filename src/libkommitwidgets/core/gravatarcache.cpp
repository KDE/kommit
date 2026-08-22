/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>
SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gravatarcache.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>

GravatarCache::GravatarCache(QObject *parent)
    : QObject{parent}
{
    const auto path = cacheLocalPath();
    QDir d;
    d.mkpath(path);
}

GravatarCache::~GravatarCache() = default;

GravatarCache *GravatarCache::instance()
{
    static GravatarCache instance;
    return &instance;
}

QString GravatarCache::cacheLocalPath() const
{
    const auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/avatars");
    return path;
}

QString GravatarCache::avatarPath(const QString &email)
{
    const auto emailHash = QCryptographicHash::hash(email.toUtf8(), QCryptographicHash::Md5).toHex().toLower();

    if (mAvatarsCache.contains(emailHash))
        return mAvatarsCache[emailHash];

    const auto avatarUrl = QStringLiteral("https://www.gravatar.com/avatar/%1").arg(QString::fromLatin1(emailHash));

    QNetworkRequest request{QUrl{avatarUrl}};

    QNetworkReply *reply = mNet.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, emailHash, reply, email]() {
        reply->deleteLater();

        // Nothing came back, which happens whenever there is no way out to the network. Read
        // it anyway and Qt says so itself, on the terminal, once per picture that was wanted.
        if (reply->error() != QNetworkReply::NoError)
            return;

        const auto avatar = reply->readAll();
        if (avatar.isEmpty())
            return;

        const QString avatarFileName{cacheLocalPath() + QLatin1Char('/') + emailHash};
        QFile avatarFile{avatarFileName};
        if (!avatarFile.open(QIODevice::WriteOnly))
            return;

        avatarFile.write(avatar);
        avatarFile.close();

        mAvatarsCache.insert(emailHash, avatarFileName);
        Q_EMIT avatarUpdated(avatarFileName, email);
    });
    return {};
}

#include "moc_gravatarcache.cpp"
