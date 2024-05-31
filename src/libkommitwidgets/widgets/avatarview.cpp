/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>
SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "avatarview.h"
#include "core/gravatarcache.h"

#include <QDir>

AvatarView::AvatarView(QWidget *parent)
    : QLabel{parent}
{
    setScaledContents(true);
}

void AvatarView::updatePixmap(const QString &userEmail)
{
    connect(GravatarCache::instance(), &GravatarCache::avatarUpdated, this, [this, userEmail](const QString &fileName, const QString &email) {
        if (userEmail == email) {
            setPixmap(QPixmap{fileName});
        }
    });
}

void AvatarView::setUserEmail(const QString &userEmail)
{
    const QString avatarFileName = GravatarCache::instance()->avatarPath(userEmail);
    if (!avatarFileName.isEmpty()) {
        if (QFile::exists(avatarFileName)) {
            setPixmap(QPixmap{avatarFileName});
        } else {
            updatePixmap(userEmail);
        }
    } else {
        updatePixmap(userEmail);
    }
}

#include "moc_avatarview.cpp"
