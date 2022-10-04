/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"

class RemotesActions : public AbstractActions
{
    Q_OBJECT

    DEFINE_ACTION(actionCreate)
    DEFINE_ACTION(actionRemove)
    DEFINE_ACTION(actionRename)
    DEFINE_ACTION(actionUpdate)
    DEFINE_ACTION(actionChangeUrl)

public:
    explicit RemotesActions(Git::Manager *git, QWidget *parent = nullptr);

    Q_REQUIRED_RESULT const QString &remoteName() const;
    void setRemoteName(const QString &newRemoteName);

private Q_SLOTS:
    void create();
    void remove();
    void changeUrl();
    void rename();
    void update();

private:
    QString mRemoteName;
};
