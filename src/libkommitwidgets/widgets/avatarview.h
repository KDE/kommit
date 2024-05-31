/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>
SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QLabel>

class AvatarView : public QLabel
{
    Q_OBJECT

public:
    explicit AvatarView(QWidget *parent = nullptr);

    Q_REQUIRED_RESULT QString userEmail() const;
    void setUserEmail(const QString &userEmail);

private:
    QString mUserEmail;
};
