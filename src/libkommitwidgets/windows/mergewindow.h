/*
Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appmainwindow.h"
#include "ui_kommitmergeview.h"
#include <diff.h>

#include "libkommitwidgets_export.h"

namespace Git
{
class Manager;
};

class SegmentsMapper;
class QLabel;
class LIBKOMMITWIDGETS_EXPORT MergeWindow : public AppMainWindow
{
    Q_OBJECT
public:
    enum Mode {
        NoParams,
        MergeByParams,
    };

    explicit MergeWindow(Git::Manager *git, Mode mode = NoParams, QWidget *parent = nullptr);
    ~MergeWindow() override;

    void load();

    [[nodiscard]] const QString &filePathLocal() const;
    void setFilePathLocal(const QString &newFilePathLocal);

    [[nodiscard]] const QString &filePathRemote() const;
    void setFilePathRemote(const QString &newFilePathRemote);

    [[nodiscard]] const QString &filePathBase() const;
    void setFilePathBase(const QString &newFilePathBase);

    [[nodiscard]] const QString &filePathResult() const;
    void setFilePathResult(const QString &newFilePathResult);

private Q_SLOTS:
    void fileSave();
    void fileOpen();
    void fillSegments();

    void actionKeepMineClicked();
    void actionKeepTheirClicked();
    void actionKeepMineBeforeTheirClicked();
    void actionKeepTheirBeforeMineClicked();
    void actionKeepMyFileClicked();
    void actionKeepTheirFileClicked();
    void actionGotoPrevDiffClicked();
    void actionGotoNextDiffClicked();

    void actionViewFilesClicked();
    void actionViewBlocksClicked();

    void codeEditorsCustomContextMenuRequested(QPoint pos);

    void slotPlainTextEditResultTextChanged();
    void slotPlainTextEditResultBlockSelected();

private:
    Ui::MainMergeWidget m_ui;
    LIBKOMMITWIDGETS_NO_EXPORT void updateResult();
    LIBKOMMITWIDGETS_NO_EXPORT void initActions();
    LIBKOMMITWIDGETS_NO_EXPORT void init();
    LIBKOMMITWIDGETS_NO_EXPORT void doMergeAction(Diff::MergeType type);
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT bool isFullyResolved() const;

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
