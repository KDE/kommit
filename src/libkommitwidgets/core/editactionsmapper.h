/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include "libkommitwidgets_export.h"

class QAction;
class QPlainTextEdit;
class KActionCollection;
class LIBKOMMITWIDGETS_EXPORT EditActionsMapper : public QObject
{
    Q_OBJECT

public:
    explicit EditActionsMapper(QObject *parent = nullptr);
    ~EditActionsMapper() override = default;

    void addTextEdit(QPlainTextEdit *control);
    void init(KActionCollection *actionCollection);

    Q_REQUIRED_RESULT bool eventFilter(QObject *watched, QEvent *event) override;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void controlUndoAvailable(bool b);
    LIBKOMMITWIDGETS_NO_EXPORT void controlRedoAvailable(bool b);
    LIBKOMMITWIDGETS_NO_EXPORT void controlCopyAvailable(bool b);
    LIBKOMMITWIDGETS_NO_EXPORT void controlSelectionChanged();

    LIBKOMMITWIDGETS_NO_EXPORT void actionUndoTriggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionRedoTriggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionCopyTriggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionCutTriggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionPasteTriggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionSelectAllTriggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionDeleteTriggered();
    QList<QPlainTextEdit *> mTextEdits;
    QPlainTextEdit *mActiveControl{nullptr};

    QAction *mActionCut{nullptr};
    QAction *mActionCopy{nullptr};
    QAction *mActionPaste{nullptr};
    QAction *mSelectAll{nullptr};
    QAction *mActionUndo{nullptr};
    QAction *mActionRedo{nullptr};
};
