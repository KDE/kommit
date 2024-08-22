/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <KSyntaxHighlighting/Repository>
#include <QMap>
#include <QPlainTextEdit>
#include <QTextBlockFormat>
#include <diff.h>

#include "libkommitwidgets_export.h"

class QLabel;

namespace KSyntaxHighlighting
{
class SyntaxHighlighter;
}

class CodeEditorSidebar;

class LIBKOMMITWIDGETS_EXPORT CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    enum BlockType { Unchanged, Added, Removed, Edited, HighLight, Odd, Even, Empty };
    struct BlockData {
        int lineNumber;
        int lineCount;

        Diff::Segment *segment;
        BlockType type;
        QString extraText;
        void *data;

        BlockData(int lineNumber, Diff::Segment *segment, CodeEditor::BlockType type);
    };

    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor() override;

    void setHighlighting(const QString &fileName);

    void append(const QString &code, CodeEditor::BlockType type = Unchanged, Diff::Segment *segment = nullptr);
    int append(const QString &code, const QColor &backgroundColor);
    void append(const QStringList &code, CodeEditor::BlockType type = Unchanged, Diff::Segment *segment = nullptr, int size = -1);
    void append(const QList<QStringView> &code, CodeEditor::BlockType type = Unchanged, Diff::Segment *segment = nullptr, int size = -1);
    int append(const QString &code, CodeEditor::BlockType type, BlockData *data);

    QPair<int, int> blockArea(int from, int to);
    QPair<int, int> visibleLines() const;

    [[nodiscard]] int currentLineNumber() const;
    void gotoLineNumber(int lineNumber);
    void gotoSegment(Diff::Segment *segment);

    Diff::Segment *currentSegment() const;
    void highlightSegment(Diff::Segment *segment);

    void clearAll();

    [[nodiscard]] bool showTitleBar() const;
    void setShowTitleBar(bool newShowTitleBar);

    [[nodiscard]] QString title() const;
    void setTitle(const QString &title);

    [[nodiscard]] int titlebarHeight() const;

    [[nodiscard]] bool showFoldMarks() const;
    void setShowFoldMarks(bool newShowFoldMarks);

    [[nodiscard]] BlockData *currentBlockData() const;

Q_SIGNALS:
    void blockSelected();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *e) override;
    int sidebarWidth() const;
    void sidebarPaintEvent(QPaintEvent *event);
    KSyntaxHighlighting::SyntaxHighlighter *const mHighlighter;
    CodeEditorSidebar *const mSideBar;

private:
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT int lineNumberOfBlock(const QTextBlock &block) const;

    LIBKOMMITWIDGETS_NO_EXPORT void setTheme(const KSyntaxHighlighting::Theme &theme);

    LIBKOMMITWIDGETS_NO_EXPORT void updateViewPortGeometry();
    LIBKOMMITWIDGETS_NO_EXPORT void updateSidebarArea(const QRect &rect, int dy);
    LIBKOMMITWIDGETS_NO_EXPORT void highlightCurrentLine();

    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT QTextBlock blockAtPosition(int y) const;
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT bool isFoldable(const QTextBlock &block) const;
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT bool isFolded(const QTextBlock &block) const;
    LIBKOMMITWIDGETS_NO_EXPORT void toggleFold(const QTextBlock &block);

    QMap<BlockType, QTextBlockFormat> mFormats;
    KSyntaxHighlighting::Repository mRepository;
    QMap<int, Diff::Segment *> mSegments;
    QMap<QTextBlock, BlockData *> mBlocksData;
    QList<BlockData *> mBlocks;

    QPair<int, int> mCurrentSegment{-1, -1};

    QLabel *mTitleBar = nullptr;
    bool mShowTitlebar = true;
    int mTitlebarDefaultHeight;

    int mLastLineNumber{0};
    bool mShowFoldMarks{false};
    bool mLastOddEven{false};

    friend class CodeEditorSidebar;
};
