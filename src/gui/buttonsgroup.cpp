/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "buttonsgroup.h"

#include <QAbstractButton>

ButtonsGroup::ButtonsGroup(QObject *parent)
    : QObject(parent)
{
}

void ButtonsGroup::addButton(QAbstractButton *btn, QObject *data)
{
    btn->setCheckable(true);
    btn->setChecked(!mButtons.size());
    connect(btn, &QAbstractButton::clicked, this, &ButtonsGroup::buttonClicked);
    if (data)
        mData.insert(btn, data);
    mButtons.append(btn);
}

QAbstractButton *ButtonsGroup::at(int i) const
{
    return mButtons.at(i);
}

void ButtonsGroup::simulateClickOn(int index)
{
    Q_EMIT clicked(index);
}

void ButtonsGroup::buttonClicked()
{
    auto btn = qobject_cast<QAbstractButton *>(sender());
    if (!btn)
        return;

    for (auto &b : std::as_const(mButtons))
        b->setChecked(b == btn);

    auto index = mButtons.indexOf(btn);
    Q_EMIT clicked(index);
}
