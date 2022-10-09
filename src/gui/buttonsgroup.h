/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include <QList>
#include <QMap>

class QAbstractButton;
class ButtonsGroup : public QObject
{
    Q_OBJECT

public:
    explicit ButtonsGroup(QObject *parent = nullptr);
    void addButton(QAbstractButton *btn, QObject *data = nullptr);
    QAbstractButton *at(int i) const;
    void simulateClickOn(int index);

private Q_SLOTS:
    void buttonClicked();

Q_SIGNALS:
    void clicked(int index);

private:
    QList<QAbstractButton *> mButtons;
    QMap<QAbstractButton *, QObject *> mData;
};
