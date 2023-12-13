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

private Q_SLOTS:
    void control_undoAvailable(bool b);
    void control_redoAvailable(bool b);
    void control_copyAvailable(bool b);
    void control_selectionChanged();

    void actionUndo_triggered();
    void actionRedo_triggered();
    void actionCopy_triggered();
    void actionCut_triggered();
    void actionPaste_triggered();
    void actionSelectAll_triggered();
    void actionDelete_triggered();

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPlainTextEdit *> mTextEdits;
    QPlainTextEdit *mActiveControl{nullptr};

    QAction *mActionCut{nullptr};
    QAction *mActionCopy{nullptr};
    QAction *mActionPaste{nullptr};
    QAction *mSelectAll{nullptr};
    QAction *mActionUndo{nullptr};
    QAction *mActionRedo{nullptr};
};
