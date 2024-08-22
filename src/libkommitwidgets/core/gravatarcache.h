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
    QNetworkAccessManager mNet;
    QMap<QString, QString> mAvatarsCache;
};
