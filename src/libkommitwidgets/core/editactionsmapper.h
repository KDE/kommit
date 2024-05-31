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

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void control_undoAvailable(bool b);
    LIBKOMMITWIDGETS_NO_EXPORT void control_redoAvailable(bool b);
    LIBKOMMITWIDGETS_NO_EXPORT void control_copyAvailable(bool b);
    LIBKOMMITWIDGETS_NO_EXPORT void control_selectionChanged();

    LIBKOMMITWIDGETS_NO_EXPORT void actionUndo_triggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionRedo_triggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionCopy_triggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionCut_triggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionPaste_triggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionSelectAll_triggered();
    LIBKOMMITWIDGETS_NO_EXPORT void actionDelete_triggered();
    QList<QPlainTextEdit *> mTextEdits;
    QPlainTextEdit *mActiveControl{nullptr};

    QAction *mActionCut{nullptr};
    QAction *mActionCopy{nullptr};
    QAction *mActionPaste{nullptr};
    QAction *mSelectAll{nullptr};
    QAction *mActionUndo{nullptr};
    QAction *mActionRedo{nullptr};
};
