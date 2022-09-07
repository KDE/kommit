/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CHANGEDFILEACTIONS_H
#define CHANGEDFILEACTIONS_H

#include "abstractactions.h"

class ChangedFileActions : public AbstractActions
{
    Q_OBJECT

    QString _filePath;

    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionRevert)

public:
    explicit ChangedFileActions(Git::Manager *git, QWidget *parent = nullptr);

    const QString &filePath() const;
    void setFilePath(const QString &newFilePath);

public slots:
    void diff();
    void revert();

signals:
    void reloadNeeded();
};

#endif // CHANGEDFILEACTIONS_H
