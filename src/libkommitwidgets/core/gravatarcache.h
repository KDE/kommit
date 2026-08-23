/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>
SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libkommitwidgets_export.h"
#include <QMap>
#include <QNetworkAccessManager>
#include <QString>

class LIBKOMMITWIDGETS_EXPORT GravatarCache : public QObject
{
    Q_OBJECT

public:
    explicit GravatarCache(QObject *parent = nullptr);
    ~GravatarCache() override;

    static GravatarCache *instance();

    [[nodiscard]] QString avatarPath(const QString &email);

    [[nodiscard]] QString cacheLocalPath() const;

Q_SIGNALS:
    void avatarUpdated(const QString &fileName, const QString &email);

private:
    /**
     * Whether there is a way out to the network to ask for a picture through.
     *
     * A machine with no way out answers every request with a failure, and Qt says so on the
     * terminal for each of them, one line per picture wanted. Asking beforehand is what keeps
     * that quiet. Where Qt cannot tell, this says yes and the request is made as before.
     */
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT bool canReachTheNetwork();

    QNetworkAccessManager mNet;
    QMap<QString, QString> mAvatarsCache;
    /// Set once a request has failed, so the ones after it are not made at all.
    bool mRequestsFail{false};
};
