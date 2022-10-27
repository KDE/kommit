/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"

#include <KService>

class FileActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit FileActions(Git::Manager *git, QWidget *parent = nullptr);
    void popup(const QPoint &pos);

    Q_REQUIRED_RESULT const QString &place() const;
    void setPlace(const QString &newPlace);

    Q_REQUIRED_RESULT const QString &filePath() const;
    void setFilePath(const QString &newFilePath);

private:
    void viewFile();
    void saveAsFile();
    void logFile();
    void blameFile();
    void search();
    void openWith();
    void diffWithHead();
    void mergeWithHead();

    DEFINE_ACTION(actionSaveAs)

    DEFINE_ACTION(actionView)
    DEFINE_ACTION(actionHistory)
    DEFINE_ACTION(actionBlame)
    DEFINE_ACTION(actionSearch)
    DEFINE_ACTION(actionOpenWith)
    DEFINE_ACTION(actionDiffWithHead)
    DEFINE_ACTION(actionMergeWithHead)
    QString mPlace;
    QString mFilePath;
    QMenu *mOpenWithMenu = nullptr;

    KService::Ptr getViewer(const QString &mimeType);
};
