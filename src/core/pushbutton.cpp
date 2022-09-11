/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "pushbutton.h"

#include <QAction>

PushButton::PushButton(QWidget *parent) : QPushButton(parent)
{

}

QAction *PushButton::action() const
{
    return mAction;
}

void PushButton::setAction(QAction *newAction)
{

    if (mAction && mAction != newAction) {
        disconnect(mAction, &QAction::changed,
                   this, &PushButton::updateButtonStatusFromAction);
        disconnect(this, &PushButton::clicked,
                   mAction, &QAction::trigger);
    }
    mAction = newAction;

    connect(mAction, &QAction::changed,
            this, &PushButton::updateButtonStatusFromAction);
    connect(this, &PushButton::clicked,
            mAction, &QAction::trigger);

    updateButtonStatusFromAction();
}

void PushButton::updateButtonStatusFromAction() {

    if (!mAction)
        return;
    setText(mAction->text());
    setStatusTip(mAction->statusTip());
    setToolTip(mAction->toolTip());
    setIcon(mAction->icon());
    setEnabled(mAction->isEnabled());
    setCheckable(mAction->isCheckable());
    setChecked(mAction->isChecked());
}
