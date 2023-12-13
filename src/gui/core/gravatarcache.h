/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMap>
#include <QNetworkAccessManager>
#include <QString>

class GravatarCache : public QObject
{
    Q_OBJECT

public:
    GravatarCache(QObject *parent = nullptr);
    QString avatarPath(const QString &email);

private:
    QNetworkAccessManager mNet;
    QMap<QString, QString> mAvatarsCache;
};
