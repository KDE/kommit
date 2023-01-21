/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "gitfile.h"
#include "ui_diffwidget.h"
#include "widgetbase.h"
#include <QTextOption>

class CodeEditor;
class DiffWidget : public WidgetBase, private Ui::DiffWIdget
{
    Q_OBJECT

    Q_PROPERTY(bool sameSize READ sameSize WRITE setSameSize NOTIFY sameSizeChanged)

public:
    explicit DiffWidget(QWidget *parent = nullptr);
    DiffWidget(const Git::File &oldFile, const Git::File &newFile, QWidget *parent = nullptr);

    const Git::File &oldFile() const;
    void setOldFile(const Git::File &newOldFile);
    const Git::File &newFile() const;
    void setNewFile(const Git::File &newNewFile);

    void compare();

    CodeEditor *oldCodeEditor() const;

    CodeEditor *newCodeEditor() const;

    bool sameSize() const;
    void setSameSize(bool newSameSize);

    void scrollToTop();

    void setOldFileText(const Git::File &newOldFile);

    void setNewFileText(const Git::File &newNewFile);

public Q_SLOTS:
    void showHiddenChars(bool show);
    void showFilesInfo(bool show);
    void showSameSize(bool show);

Q_SIGNALS:
    void sameSizeChanged();

private Q_SLOTS:
    void slotSegmentsScrollbarHover(int y, double pos);
    void slotSplitterSplitterMoved(int, int);
    void oldCodeEditor_scroll(int value);
    void newCodeEditor_scroll(int value);
    void oldCodeEditor_blockSelected();
    void newCodeEditor_blockSelected();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    constexpr static int mPreviewWidgetHeight{160};
    QWidget *mPreviewWidget;
    int mPreviewMargin{0};
    CodeEditor *mPreviewEditorLeft;
    CodeEditor *mPreviewEditorRight;
    bool mSameSize{false};
    Git::File mOldFile;
    Git::File mNewFile;

    QTextOption mDefaultOption;

    void recalculateInfoPaneSize();
    void init();
    void createPreviewWidget();
};
