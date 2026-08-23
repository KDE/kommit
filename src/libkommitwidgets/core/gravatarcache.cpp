/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>
SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gravatarcache.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <algorithm>

#include <QNetworkInterface>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QStandardPaths>
#include <QtConcurrent>

GravatarCache::GravatarCache(QObject *parent)
    : QObject{parent}
{
    const auto path = cacheLocalPath();
    QDir d;
    d.mkpath(path);

    // Qt reads and parses the whole store of certificate authorities the first time anything
    // asks for a standard secure configuration, which the first request for a picture does. On
    // the thread that draws the window that took three seconds, at the moment a commit was
    // picked and its author's picture was asked for. Asked for here instead, on a thread of its
    // own, so that by the time a picture is wanted the parsing is behind us.
    static auto warmed = QtConcurrent::run([] {
        QSslConfiguration::defaultConfiguration().caCertificates();
    });
    Q_UNUSED(warmed)
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

bool GravatarCache::canReachTheNetwork()
{
    if (mRequestsFail)
        return false;

    // Whether anything but the machine itself can be reached at all. Asked of the interfaces
    // rather than of QNetworkInformation, whose backend is a plugin that talks over the bus and
    // holds what it makes there for as long as the program runs.
    const auto interfaces = QNetworkInterface::allInterfaces();

    return std::any_of(interfaces.constBegin(), interfaces.constEnd(), [](const QNetworkInterface &interface) {
        const auto flags = interface.flags();
        return flags.testFlag(QNetworkInterface::IsUp) && flags.testFlag(QNetworkInterface::IsRunning) && !flags.testFlag(QNetworkInterface::IsLoopBack);
    });
}

QString GravatarCache::avatarPath(const QString &email)
{
    const auto emailHash = QCryptographicHash::hash(email.toUtf8(), QCryptographicHash::Md5).toHex().toLower();

    if (mAvatarsCache.contains(emailHash))
        return mAvatarsCache[emailHash];

    if (!canReachTheNetwork())
        return {};

    const auto avatarUrl = QStringLiteral("https://www.gravatar.com/avatar/%1").arg(QString::fromLatin1(emailHash));

    QNetworkRequest request{QUrl{avatarUrl}};

    QNetworkReply *reply = mNet.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, emailHash, reply, email]() {
        reply->deleteLater();

        // Nothing came back, which happens whenever there is no way out to the network. The
        // ones after this are not asked for at all, since they would go the same way and Qt
        // says so on the terminal every time.
        if (reply->error() != QNetworkReply::NoError) {
            mRequestsFail = true;
            return;
        }

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
