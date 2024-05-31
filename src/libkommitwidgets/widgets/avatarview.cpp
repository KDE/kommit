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

QString AvatarView::userEmail() const
{
    return mUserEmail;
}

void AvatarView::setUserEmail(const QString &userEmail)
{
    mUserEmail = userEmail;

    const QString avatarFileName = GravatarCache::instance()->avatarPath(userEmail);
    if (!avatarFileName.isEmpty()) {
        if (QFile::exists(avatarFileName)) {
            setPixmap(QPixmap{avatarFileName});
        }
    } else {
        connect(GravatarCache::instance(), &GravatarCache::avatarUpdated, this, [this, userEmail](const QString &fileName, const QString &email) {
            if (userEmail == email) {
                setPixmap(QPixmap{fileName});
            }
        });
    }
}

#include "moc_avatarview.cpp"
