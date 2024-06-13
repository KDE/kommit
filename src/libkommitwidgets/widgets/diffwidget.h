/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_diffwidget.h"

#include <QTextOption>
#include <QWidget>
#include <entities/file.h>

#include "libkommitwidgets_export.h"

class CodeEditor;
class LIBKOMMITWIDGETS_EXPORT DiffWidget : public QWidget, private Ui::DiffWIdget
{
    Q_OBJECT

    Q_PROPERTY(bool sameSize READ sameSize WRITE setSameSize NOTIFY sameSizeChanged)

public:
    explicit DiffWidget(QWidget *parent = nullptr);
    DiffWidget(QSharedPointer<Git::File> oldFile, QSharedPointer<Git::File> newFile, QWidget *parent = nullptr);
    ~DiffWidget();

    QSharedPointer<Git::File> oldFile() const;
    void setOldFile(QSharedPointer<Git::File> newOldFile);
    QSharedPointer<Git::File> newFile() const;
    void setNewFile(QSharedPointer<Git::File> newNewFile);

    void compare();

    CodeEditor *oldCodeEditor() const;

    CodeEditor *newCodeEditor() const;

    Q_REQUIRED_RESULT bool sameSize() const;
    void setSameSize(bool newSameSize);

    void scrollToTop();

    void setOldFileText(const QString &newOldFile);
    void setNewFileText(const QString &newNewFile);

public Q_SLOTS:
    void showHiddenChars(bool show);
    void showFilesInfo(bool show);
    void showSameSize(bool show);

Q_SIGNALS:
    void sameSizeChanged();

private Q_SLOTS:
    LIBKOMMITWIDGETS_NO_EXPORT void slotSegmentsScrollbarHover(int y, double pos);
    LIBKOMMITWIDGETS_NO_EXPORT void slotSplitterSplitterMoved(int, int);
    LIBKOMMITWIDGETS_NO_EXPORT void oldCodeEditor_scroll(int value);
    LIBKOMMITWIDGETS_NO_EXPORT void newCodeEditor_scroll(int value);
    LIBKOMMITWIDGETS_NO_EXPORT void oldCodeEditor_blockSelected();
    LIBKOMMITWIDGETS_NO_EXPORT void newCodeEditor_blockSelected();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    bool mDestroying{false};
    constexpr static int mPreviewWidgetHeight{160};
    QWidget *mPreviewWidget = nullptr;
    int mPreviewMargin{0};
    CodeEditor *mPreviewEditorLeft = nullptr;
    CodeEditor *mPreviewEditorRight = nullptr;
    bool mSameSize{false};
    QSharedPointer<Git::File> mOldFile;
    QSharedPointer<Git::File> mNewFile;

    QTextOption mDefaultOption;

    void recalculateInfoPaneSize();
    void init();
    void createPreviewWidget();
};
