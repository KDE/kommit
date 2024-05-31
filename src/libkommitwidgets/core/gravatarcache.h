/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

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

    Q_REQUIRED_RESULT QString avatarPath(const QString &email);

private:
    QNetworkAccessManager mNet;
    QMap<QString, QString> mAvatarsCache;
};
