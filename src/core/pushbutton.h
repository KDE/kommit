/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>

class PushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit PushButton(QWidget *parent = nullptr);
    QAction *action() const;
    void setAction(QAction *newAction);

private:
    void updateButtonStatusFromAction();
    QAction *_action{nullptr};
};

#endif // PUSHBUTTON_H
