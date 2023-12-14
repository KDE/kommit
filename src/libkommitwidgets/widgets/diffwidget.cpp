/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diffwidget.h"
#include "codeeditor.h"
#include <diff.h>

#include <QScrollBar>
#include <QTextBlock>

DiffWidget::DiffWidget(QWidget *parent)
    : QWidget{parent}
    , mOldFile()
    , mNewFile()
{
    setupUi(this);
    init();
}

DiffWidget::DiffWidget(QSharedPointer<Git::File> oldFile, QSharedPointer<Git::File> newFile, QWidget *parent)
    : QWidget{parent}
    , mOldFile(oldFile)
    , mNewFile(newFile)
{
    setupUi(this);
    init();
}

void DiffWidget::init()
{
    createPreviewWidget();
    segmentConnector->setMinimumWidth(80);
    segmentConnector->setMaximumWidth(80);
    segmentConnector->setLeft(leftCodeEditor);
    segmentConnector->setRight(rightCodeEditor);

    widgetSegmentsScrollBar->setSegmentConnector(segmentConnector);

    connect(leftCodeEditor, &CodeEditor::blockSelected, this, &DiffWidget::oldCodeEditor_blockSelected);
    connect(rightCodeEditor, &CodeEditor::blockSelected, this, &DiffWidget::newCodeEditor_blockSelected);
    connect(leftCodeEditor->verticalScrollBar(), &QScrollBar::valueChanged, this, &DiffWidget::oldCodeEditor_scroll);
    connect(rightCodeEditor->verticalScrollBar(), &QScrollBar::valueChanged, this, &DiffWidget::newCodeEditor_scroll);
    connect(splitter, &QSplitter::splitterMoved, this, &DiffWidget::slotSplitterSplitterMoved);

    recalculateInfoPaneSize();

    mDefaultOption = leftCodeEditor->document()->defaultTextOption();

    connect(widgetSegmentsScrollBar, &SegmentsScrollBar::hover, this, &DiffWidget::slotSegmentsScrollbarHover);
    connect(widgetSegmentsScrollBar, &SegmentsScrollBar::mouseEntered, mPreviewWidget, &QWidget::show);
    connect(widgetSegmentsScrollBar, &SegmentsScrollBar::mouseLeaved, mPreviewWidget, &QWidget::hide);
    showFilesInfo(true);
}

void DiffWidget::createPreviewWidget()
{
    mPreviewWidget = new QWidget(this);
    auto layout = new QHBoxLayout(mPreviewWidget);
    mPreviewEditorLeft = new CodeEditor(mPreviewWidget);
    mPreviewEditorLeft->setShowFoldMarks(false);
    mPreviewEditorLeft->setShowTitleBar(false);
    mPreviewEditorLeft->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mPreviewEditorLeft->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(mPreviewEditorLeft);

    mPreviewEditorRight = new CodeEditor(mPreviewWidget);
    mPreviewEditorRight->setShowFoldMarks(false);
    mPreviewEditorRight->setShowTitleBar(false);
    mPreviewEditorRight->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mPreviewEditorRight->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(mPreviewEditorRight);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(segmentConnector->width() + 2 * (splitter->handleWidth()));
    mPreviewWidget->setLayout(layout);
    mPreviewWidget->hide();
}
QSharedPointer<Git::File> DiffWidget::oldFile() const
{
    return mOldFile;
}

void DiffWidget::setOldFileText(const QString &newOldFile)
{
    leftCodeEditor->setTitle(newOldFile);
}

void DiffWidget::setOldFile(QSharedPointer<Git::File> newOldFile)
{
    mOldFile = newOldFile;
    setOldFileText(newOldFile->displayName());
}

QSharedPointer<Git::File> DiffWidget::newFile() const
{
    return mNewFile;
}

void DiffWidget::setNewFileText(const QString &newNewFile)
{
    rightCodeEditor->setTitle(newNewFile);
}

void DiffWidget::setNewFile(QSharedPointer<Git::File> newNewFile)
{
    mNewFile = newNewFile;
    setNewFileText(newNewFile->displayName());
}

void DiffWidget::compare()
{
    if (!mOldFile || !mNewFile)
        return;

    const auto segments = Diff::diff(mOldFile->content(), mNewFile->content());

    leftCodeEditor->clearAll();
    rightCodeEditor->clearAll();

    mPreviewEditorLeft->clearAll();
    mPreviewEditorRight->clearAll();

    leftCodeEditor->setHighlighting(mOldFile->fileName());
    rightCodeEditor->setHighlighting(mNewFile->fileName());

    mPreviewEditorLeft->setHighlighting(mOldFile->fileName());
    mPreviewEditorRight->setHighlighting(mNewFile->fileName());

    segmentConnector->setSegments(segments);
    segmentConnector->update();

    for (const auto &s : segments) {
        CodeEditor::BlockType oldBlockType, newBlockType;
        switch (s->type) {
        case Diff::SegmentType::SameOnBoth:
            oldBlockType = newBlockType = CodeEditor::Unchanged;
            break;
        case Diff::SegmentType::OnlyOnLeft:
            oldBlockType = CodeEditor::Removed;
            newBlockType = CodeEditor::Added;
            break;
        case Diff::SegmentType::OnlyOnRight:
            oldBlockType = CodeEditor::Removed;
            newBlockType = CodeEditor::Added;
            break;
        case Diff::SegmentType::DifferentOnBoth:
            oldBlockType = newBlockType = CodeEditor::Edited;
            break;
        }

        if (mSameSize) {
            const int size = qMax(s->oldText.size(), s->newText.size());
            leftCodeEditor->append(s->oldText, oldBlockType, s, size);
            rightCodeEditor->append(s->newText, newBlockType, s, size);

            mPreviewEditorLeft->append(s->oldText, oldBlockType, s, size);
            mPreviewEditorRight->append(s->newText, newBlockType, s, size);
        } else {
            leftCodeEditor->append(s->oldText, oldBlockType, s);
            rightCodeEditor->append(s->newText, newBlockType, s);

            mPreviewEditorLeft->append(s->oldText, oldBlockType, s);
            mPreviewEditorRight->append(s->newText, newBlockType, s);
        }
    }

    scrollToTop();
}

void DiffWidget::showHiddenChars(bool show)
{
    if (show) {
        auto n = mDefaultOption;
        n.setFlags(QTextOption::ShowTabsAndSpaces | QTextOption::ShowDocumentTerminator);
        leftCodeEditor->document()->setDefaultTextOption(n);
        rightCodeEditor->document()->setDefaultTextOption(n);
    } else {
        leftCodeEditor->document()->setDefaultTextOption(mDefaultOption);
        rightCodeEditor->document()->setDefaultTextOption(mDefaultOption);
    }
    leftCodeEditor->setWordWrapMode(QTextOption::NoWrap);
    rightCodeEditor->setWordWrapMode(QTextOption::NoWrap);
}

void DiffWidget::showFilesInfo(bool show)
{
    leftCodeEditor->setShowTitleBar(show);
    rightCodeEditor->setShowTitleBar(show);
    segmentConnector->setTopMargin(show ? leftCodeEditor->titlebarHeight() : 0);
}

void DiffWidget::showSameSize(bool show)
{
    mSameSize = show;
    segmentConnector->setSameSize(show);
    compare();
}

void DiffWidget::slotSegmentsScrollbarHover(int y, double pos)
{
    mPreviewWidget->show();
    mPreviewWidget->move(mPreviewMargin, qMin(y, widgetSegmentsScrollBar->height() - mPreviewWidgetHeight));

    mPreviewEditorLeft->verticalScrollBar()->setValue(pos * static_cast<double>(mPreviewEditorLeft->verticalScrollBar()->maximum()));
    mPreviewEditorRight->verticalScrollBar()->setValue(pos * mPreviewEditorRight->verticalScrollBar()->maximum());
}

void DiffWidget::slotSplitterSplitterMoved(int, int)
{
    recalculateInfoPaneSize();
}

CodeEditor *DiffWidget::oldCodeEditor() const
{
    return leftCodeEditor;
}

CodeEditor *DiffWidget::newCodeEditor() const
{
    return rightCodeEditor;
}

void DiffWidget::oldCodeEditor_scroll(int value)
{
    static bool b{false};
    if (b)
        return;
    b = true;
    rightCodeEditor->verticalScrollBar()->setValue(
        (int)(((float)value / (float)rightCodeEditor->verticalScrollBar()->maximum()) * (float)rightCodeEditor->verticalScrollBar()->maximum()));
    b = false;
    segmentConnector->update();
    widgetSegmentsScrollBar->update();
}

void DiffWidget::newCodeEditor_scroll(int value)
{
    static bool b{false};
    if (b)
        return;
    b = true;
    leftCodeEditor->verticalScrollBar()->setValue(
        (int)(((float)value / (float)leftCodeEditor->verticalScrollBar()->maximum()) * (float)leftCodeEditor->verticalScrollBar()->maximum()));
    b = false;
    segmentConnector->update();
    widgetSegmentsScrollBar->update();
}

void DiffWidget::oldCodeEditor_blockSelected()
{
    //    auto b = _oldCodeEditor->textCursor().block().blockNumber();
    //    auto b = _oldCodeEditor->currentSegment();
    //    if (b) {
    //        _segmentConnector->setCurrentSegment(b);
    //        _newCodeEditor->highlightSegment(b);
    //    }
}

void DiffWidget::newCodeEditor_blockSelected()
{
    //    auto b = _newCodeEditor->currentSegment();
    //    if (b) {
    //        _segmentConnector->setCurrentSegment(b);
    //        _oldCodeEditor->highlightSegment(b);
    //    }
}

void DiffWidget::recalculateInfoPaneSize()
{
    //    leftInfoContainer->setMinimumWidth(leftCodeEditor->width());
    //    rightInfoContainer->setMinimumWidth(rightCodeEditor->width());

    //    leftInfoContainer->setVisible(leftCodeEditor->width());
    //    rightInfoContainer->setVisible(rightCodeEditor->width());

    //    label->setMinimumWidth(leftCodeEditor->width());
    //    label_2->setMinimumWidth(rightCodeEditor->width());
}

void DiffWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    recalculateInfoPaneSize();
    mPreviewWidget->resize(splitter->width(), mPreviewWidgetHeight);
    mPreviewMargin = splitter->mapToParent(QPoint{0, 0}).x();
}

void DiffWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    recalculateInfoPaneSize();
}

bool DiffWidget::sameSize() const
{
    return mSameSize;
}

void DiffWidget::setSameSize(bool newSameSize)
{
    if (mSameSize == newSameSize)
        return;
    mSameSize = newSameSize;
    Q_EMIT sameSizeChanged();
}

void DiffWidget::scrollToTop()
{
    leftCodeEditor->setTextCursor(QTextCursor(leftCodeEditor->document()->findBlockByNumber(0)));
    rightCodeEditor->setTextCursor(QTextCursor(rightCodeEditor->document()->findBlockByNumber(0)));
    //    leftCodeEditor->verticalScrollBar()->setValue(0);
    //    rightCodeEditor->verticalScrollBar()->setValue(0);
    segmentConnector->update();
}

#include "moc_diffwidget.cpp"
