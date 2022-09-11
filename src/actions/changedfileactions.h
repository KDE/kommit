/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"

class ChangedFileActions : public AbstractActions
{
    Q_OBJECT

    QString mFilePath;

    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionRevert)

public:
    explicit ChangedFileActions(Git::Manager *git, QWidget *parent = nullptr);

    const QString &filePath() const;
    void setFilePath(const QString &newFilePath);

public Q_SLOTS:
    void diff();
    void revert();

Q_SIGNALS:
    void reloadNeeded();
};
