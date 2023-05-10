/*
Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appmainwindow.h"
#include "ui_kommitmergeview.h"
#include <diff.h>

namespace Git
{
class Manager;
};

class SegmentsMapper;
class QLabel;
class MergeWindow : public AppMainWindow
{
    Q_OBJECT
public:
    enum Mode { NoParams, MergeByParams };

    explicit MergeWindow(Git::Manager *git, Mode mode = NoParams, QWidget *parent = nullptr);
    ~MergeWindow() override;

    void load();

    Q_REQUIRED_RESULT const QString &filePathLocal() const;
    void setFilePathLocal(const QString &newFilePathLocal);

    Q_REQUIRED_RESULT const QString &filePathRemote() const;
    void setFilePathRemote(const QString &newFilePathRemote);

    Q_REQUIRED_RESULT const QString &filePathBase() const;
    void setFilePathBase(const QString &newFilePathBase);

    Q_REQUIRED_RESULT const QString &filePathResult() const;
    void setFilePathResult(const QString &newFilePathResult);

private Q_SLOTS:
    void fileSave();
    void fileOpen();
    void fillSegments();

    void actionKeepMine_clicked();
    void actionKeepTheir_clicked();
    void actionKeepMineBeforeTheir_clicked();
    void actionKeepTheirBeforeMine_clicked();
    void actionKeepMyFile_clicked();
    void actionKeepTheirFile_clicked();
    void actionGotoPrevDiff_clicked();
    void actionGotoNextDiff_clicked();

    void actionViewFiles_clicked();
    void actionViewBlocks_clicked();

    void codeEditors_customContextMenuRequested(QPoint pos);

    void slotPlainTextEditResultTextChanged();
    void slotPlainTextEditResultBlockSelected();

private:
    Git::Manager *const mGit;
    Ui::MainMergeWidget m_ui;
    void updateResult();
    void initActions();
    void init();
    void doMergeAction(Diff::MergeType type);
    bool isFullyResolved() const;

    QList<Diff::MergeSegment *> mDiffs;
    QMenu *mCodeEditorContextMenu = nullptr;
    SegmentsMapper *mMapper = nullptr;

    QString mFilePathLocal;
    QString mFilePathRemote;
    QString mFilePathBase;
    QString mFilePathResult;
    QLabel *mConflictsLabel = nullptr;
    QAction *mActionBlocksView = nullptr;
    QAction *mActionFilesView = nullptr;
    QAction *actionKeepMine = nullptr;
    QAction *actionKeepTheir = nullptr;
    QAction *actionKeepMineBeforeTheir = nullptr;
    QAction *actionKeepTheirBeforeMine = nullptr;
    QAction *actionKeepMyFile = nullptr;
    QAction *actionKeepTheirFile = nullptr;
    QAction *actionGotoPrevDiff = nullptr;
    QAction *actionGotoNextDiff = nullptr;
    QAction *actionViewSameSizeBlocks = nullptr;

protected:
    void closeEvent(QCloseEvent *event) override;
};
