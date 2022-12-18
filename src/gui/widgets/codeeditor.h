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

class QLabel;

namespace KSyntaxHighlighting
{
class SyntaxHighlighter;
}

class CodeEditorSidebar;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    enum BlockType { Unchanged, Added, Removed, Edited, HighLight, Odd, Even, Empty };
    struct BlockData {
        int lineNumber;
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
    int append(const QString &code, CodeEditor::BlockType type, BlockData *data);

    QPair<int, int> blockArea(int from, int to);
    QPair<int, int> visibleLines() const;

    int currentLineNumber() const;
    void gotoLineNumber(int lineNumber);
    void gotoSegment(Diff::Segment *segment);

    Diff::Segment *currentSegment() const;
    void highlightSegment(Diff::Segment *segment);

    void clearAll();

    bool showTitleBar() const;
    void setShowTitleBar(bool newShowTitleBar);

    QString title() const;
    void setTitle(const QString &title);

    int titlebarHeight() const;

    bool showFoldMarks() const;
    void setShowFoldMarks(bool newShowFoldMarks);

    BlockData *currentBlockData() const;

Q_SIGNALS:
    void blockSelected();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    int sidebarWidth() const;
    void sidebarPaintEvent(QPaintEvent *event);
    KSyntaxHighlighting::SyntaxHighlighter *const mHighlighter;
    CodeEditorSidebar *const mSideBar;

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    int lineNumberOfBlock(const QTextBlock &block) const;

    QMap<BlockType, QTextBlockFormat> mFormats;
    void setTheme(const KSyntaxHighlighting::Theme &theme);

    void updateViewPortGeometry();
    void updateSidebarArea(const QRect &rect, int dy);
    void highlightCurrentLine();

    QTextBlock blockAtPosition(int y) const;
    bool isFoldable(const QTextBlock &block) const;
    bool isFolded(const QTextBlock &block) const;
    void toggleFold(const QTextBlock &block);

    KSyntaxHighlighting::Repository mRepository;
    QMap<int, Diff::Segment *> mSegments;
    QMap<QTextBlock, BlockData *> mBlocksData;
    QPair<int, int> mCurrentSegment{-1, -1};

    QLabel *mTitleBar;
    bool mShowTitlebar = true;
    int mTitlebarDefaultHeight;

    int mLastLineNumber{0};
    bool mShowFoldMarks{false};
    bool mLastOddEven{false};

    friend class CodeEditorSidebar;
};
