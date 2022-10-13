/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"

class TagsActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit TagsActions(Git::Manager *git, QWidget *parent = nullptr);

    Q_REQUIRED_RESULT const QString &tagName() const;
    void setTagName(const QString &newTagName);

private:
    void create();
    void remove();
    void checkout();
    void diff();
    void push();
    DEFINE_ACTION(actionCreate)
    DEFINE_ACTION(actionRemove)
    DEFINE_ACTION(actionCheckout)
    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionPush)
    QString mTagName;
};
