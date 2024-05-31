/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"
#include "libkommitwidgets_export.h"

class LIBKOMMITWIDGETS_EXPORT ChangedFileActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit ChangedFileActions(Git::Manager *git, QWidget *parent = nullptr);

    Q_REQUIRED_RESULT const QString &filePath() const;
    void setFilePath(const QString &newFilePath);
    void setFilePaths(const QString &originalFilePath, const QString &renamedFilePath);
    void diff();

Q_SIGNALS:
    void reloadNeeded();

private:
    LIBKOMMITWIDGETS_NO_EXPORT void revert();
    QString mOriginalFilePath;
    QString mFilePath;

    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionRevert)
};
