/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diffwidget.h"
#include "codeeditor.h"
#include <diff.h>

#include <QFile>
#include <QFileInfo>
#include <QScrollBar>
#include <QTextBlock>

class DiffWidgetPrivate
{
    DiffWidget *q_ptr;
    Q_DECLARE_PUBLIC(DiffWidget)

public:
    explicit DiffWidgetPrivate(DiffWidget *parent);

    bool mDestroying{false};
    constexpr static int mPreviewWidgetHeight{160};
    QWidget *mPreviewWidget = nullptr;
    int mPreviewMargin{0};
    CodeEditor *mPreviewEditorLeft = nullptr;
    CodeEditor *mPreviewEditorRight = nullptr;
    bool mSameSize{false};

    Diff::Diff2TextResult lastDiff;
    // QString leftContentWithSpaces;
    // QString rightContentWithSpaces;

    QList<CodeEditor::BlockData *> leftBlockDataList;
    QList<CodeEditor::BlockData *> rightBlockDataList;

    QString mOldContent;
    QString mNewContent;
    QString mOldFileName;
    QString mNewFileName;
    QTextOption mDefaultOption;
    QList<Diff::DiffHunk *> mSegments;

    void init();
    void recalculateInfoPaneSize();
    void createPreviewWidget();
    void setEditorsContents(QList<Diff::DiffHunk *> segments);
};

DiffWidgetPrivate::DiffWidgetPrivate(DiffWidget *parent)
    : q_ptr{parent}
{
}
DiffWidget::DiffWidget(QWidget *parent)
    : QWidget{parent}
    , d_ptr{new DiffWidgetPrivate{this}}
{
    Q_D(DiffWidget);
    setupUi(this);
    d->init();
}

// DiffWidget::DiffWidget(QSharedPointer<Git::Blob> oldFile, QSharedPointer<Git::Blob> newFile, QWidget *parent)
//     : QWidget{parent}
// {
//     setupUi(this);
//     init();

//     setOldFile(oldFile);
//     setNewFile(newFile);
// }

DiffWidget::~DiffWidget()
{
    Q_D(DiffWidget);

    leftCodeEditor->clearAll();
    rightCodeEditor->clearAll();

    d->mPreviewEditorLeft->clearAll();
    d->mPreviewEditorRight->clearAll();

    d->mDestroying = true;
}

void DiffWidget::setOldFileText(const QString &newOldFile)
{
    leftCodeEditor->setTitle(newOldFile);
}

void DiffWidget::setOldFile(QSharedPointer<Git::Blob> newOldFile)
{
    setOldFile(newOldFile->name(), newOldFile->content());
}

void DiffWidget::setNewFileText(const QString &newNewFile)
{
    rightCodeEditor->setTitle(newNewFile);
}

void DiffWidget::setNewFile(QSharedPointer<Git::Blob> newNewFile)
{
    setNewFile(newNewFile->name(), newNewFile->content());
}

void DiffWidget::setOldFile(QSharedPointer<Git::File> newOldFile)
{
    setOldFile(newOldFile->fileName(), newOldFile->content());
}

void DiffWidget::setNewFile(QSharedPointer<Git::File> newNewFile)
{
    setNewFile(newNewFile->fileName(), newNewFile->content());
}

void DiffWidget::setOldFile(const QString &filePath)
{
    Q_D(DiffWidget);

    QFileInfo fi{filePath};

    if (!fi.exists())
        return;

    d->mOldFileName = fi.fileName();
    QFile f{filePath};
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    d->mOldContent = f.readAll();
    f.close();
}

void DiffWidget::setNewFile(const QString &filePath)
{
    Q_D(DiffWidget);

    QFileInfo fi{filePath};

    if (!fi.exists())
        return;

    d->mNewFileName = fi.fileName();
    QFile f{filePath};
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    d->mNewContent = f.readAll();
    f.close();
}

void DiffWidget::setOldFile(const QString &title, const QString &content)
{
    Q_D(DiffWidget);

    leftCodeEditor->setHighlighting(title);
    d->mPreviewEditorLeft->setHighlighting(title);

    d->mOldContent = content;
    d->mOldFileName = title;
}

void DiffWidget::setNewFile(const QString &title, const QString &content)
{
    Q_D(DiffWidget);

    rightCodeEditor->setHighlighting(title);
    d->mPreviewEditorRight->setHighlighting(title);

    d->mNewContent = content;
    d->mNewFileName = title;
}

void DiffWidget::compare()
{
    Q_D(DiffWidget);

    d->lastDiff = Diff::diff2(d->mOldContent, d->mNewContent);
    const auto segments = d->lastDiff.hunks();

    // QString tmpLeftContentWithSpaces;
    // QString tmpRightContentWithSpaces;

    QList<CodeEditor::BlockData *> tmpLeftBlockDataList;
    QList<CodeEditor::BlockData *> tmpRightBlockDataList;

    tmpLeftBlockDataList.reserve(segments.size());
    tmpRightBlockDataList.reserve(segments.size());

    for (auto &segment : segments) {
        CodeEditor::BlockType leftBlockType{CodeEditor::BlockType::Unchanged};
        CodeEditor::BlockType rightBlockType{CodeEditor::BlockType::Unchanged};
        switch (segment->type) {
        case Diff::SegmentType::SameOnBoth:
            // tmpLeftContentWithSpaces += segment->oldText.join('\n');
            // tmpRightContentWithSpaces += segment->newText.join('\n');
            leftBlockType = rightBlockType = CodeEditor::BlockType::Unchanged;
            break;
        case Diff::SegmentType::OnlyOnLeft:
            // tmpLeftContentWithSpaces += segment->oldText.join('\n');
            // tmpRightContentWithSpaces += QString{segment->oldText.size(), QLatin1Char('\n')};

            leftBlockType = CodeEditor::BlockType::Removed;
            rightBlockType = CodeEditor::BlockType::Edited;
            break;
        case Diff::SegmentType::OnlyOnRight:
            // tmpLeftContentWithSpaces += QString{segment->newText.size(), QLatin1Char('\n')};
            // tmpRightContentWithSpaces += segment->newText.join('\n');

            leftBlockType = CodeEditor::BlockType::Edited;
            rightBlockType = CodeEditor::BlockType::Added;
            break;
        case Diff::SegmentType::DifferentOnBoth:
            // tmpLeftContentWithSpaces += segment->oldText.join('\n');
            // tmpRightContentWithSpaces += segment->newText.join('\n');

            // if (segment->oldText.size() < segment->newText.size()) {
            //     tmpRightContentWithSpaces += QString{segment->newText.size() - segment->oldText.size(), QLatin1Char('\n')};
            // } else if (segment->oldText.size() > segment->newText.size()) {
            //     tmpLeftContentWithSpaces += QString{segment->oldText.size() - segment->newText.size(), QLatin1Char('\n')};
            // }
            leftBlockType = rightBlockType = CodeEditor::BlockType::Edited;

            break;
        }

        Q_ASSERT(segment->left.size >= 0);
        Q_ASSERT(segment->right.size >= 0);
        auto m = qMax(segment->left.size, segment->right.size);
        tmpLeftBlockDataList.append(new CodeEditor::BlockData{segment->left.begin, segment->left.size, m, leftBlockType});
        tmpRightBlockDataList.append(new CodeEditor::BlockData{segment->right.begin, segment->right.size, m, rightBlockType});

        // if (!tmpLeftContentWithSpaces.endsWith('\n'))
        //     tmpLeftContentWithSpaces == '\n';
        // if (!tmpRightContentWithSpaces.endsWith('\n'))
        //     tmpRightContentWithSpaces == '\n';
    }

    d->leftBlockDataList = tmpLeftBlockDataList;
    d->rightBlockDataList = tmpRightBlockDataList;

    // d->leftContentWithSpaces = tmpLeftContentWithSpaces;
    // d->rightContentWithSpaces = tmpRightContentWithSpaces;

    d->setEditorsContents(segments);

    // qDeleteAll(d->mSegments);
    d->mSegments = segments;
}

void DiffWidget::showHiddenChars(bool show)
{
    Q_D(DiffWidget);

    if (show) {
        auto n = d->mDefaultOption;
        n.setFlags(QTextOption::ShowTabsAndSpaces | QTextOption::ShowDocumentTerminator);
        leftCodeEditor->document()->setDefaultTextOption(n);
        rightCodeEditor->document()->setDefaultTextOption(n);
    } else {
        leftCodeEditor->document()->setDefaultTextOption(d->mDefaultOption);
        rightCodeEditor->document()->setDefaultTextOption(d->mDefaultOption);
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
    Q_D(DiffWidget);

    d->mSameSize = show;
    segmentConnector->setSameSize(show);
    // compare();
    d->setEditorsContents(d->mSegments);
}

void DiffWidget::slotSegmentsScrollbarHover(int y, double pos)
{
    Q_D(DiffWidget);

    d->mPreviewWidget->show();
    d->mPreviewWidget->move(d->mPreviewMargin, qMin(y, widgetSegmentsScrollBar->height() - d->mPreviewWidgetHeight));

    d->mPreviewEditorLeft->verticalScrollBar()->setValue(pos * static_cast<double>(d->mPreviewEditorLeft->verticalScrollBar()->maximum()));
    d->mPreviewEditorRight->verticalScrollBar()->setValue(pos * d->mPreviewEditorRight->verticalScrollBar()->maximum());
}

void DiffWidget::slotSegmentsScrollbarMouseMove(int y, double pos)
{
    Q_D(DiffWidget);

    d->mPreviewWidget->show();
    d->mPreviewWidget->move(d->mPreviewMargin, qMin(y, widgetSegmentsScrollBar->height() - d->mPreviewWidgetHeight));

    leftCodeEditor->verticalScrollBar()->setValue(pos * leftCodeEditor->verticalScrollBar()->maximum());
    rightCodeEditor->verticalScrollBar()->setValue(pos * rightCodeEditor->verticalScrollBar()->maximum());

    d->mPreviewEditorLeft->verticalScrollBar()->setValue(pos * static_cast<double>(d->mPreviewEditorLeft->verticalScrollBar()->maximum()));
    d->mPreviewEditorRight->verticalScrollBar()->setValue(pos * d->mPreviewEditorRight->verticalScrollBar()->maximum());
}

void DiffWidget::slotSplitterSplitterMoved(int, int)
{
    Q_D(DiffWidget);
    d->recalculateInfoPaneSize();
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
    Q_D(DiffWidget);
    if (Q_UNLIKELY(d->mDestroying))
        return;
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
    Q_D(DiffWidget);
    if (Q_UNLIKELY(d->mDestroying))
        return;
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

void DiffWidgetPrivate::recalculateInfoPaneSize()
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
    Q_D(DiffWidget);
    QWidget::resizeEvent(event);
    d->recalculateInfoPaneSize();
    d->mPreviewWidget->resize(splitter->width(), d->mPreviewWidgetHeight);
    d->mPreviewMargin = splitter->mapToParent(QPoint{0, 0}).x();
}

void DiffWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    Q_D(DiffWidget);
    d->recalculateInfoPaneSize();
}

bool DiffWidget::sameSize() const
{
    Q_D(const DiffWidget);
    return d->mSameSize;
}

void DiffWidget::setSameSize(bool newSameSize)
{
    Q_D(DiffWidget);
    if (d->mSameSize == newSameSize)
        return;
    d->mSameSize = newSameSize;
    d->setEditorsContents(d->mSegments);
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

void DiffWidgetPrivate::init()
{
    Q_Q(DiffWidget);

    createPreviewWidget();
    q->segmentConnector->setMinimumWidth(80);
    q->segmentConnector->setMaximumWidth(80);
    q->segmentConnector->setLeft(q->leftCodeEditor);
    q->segmentConnector->setRight(q->rightCodeEditor);

    q->widgetSegmentsScrollBar->setSegmentConnector(q->segmentConnector);

    // QObject::connect(q->leftCodeEditor, &CodeEditor::blockSelected, q, &DiffWidget::oldCodeEditor_blockSelected);
    // QObject::connect(q->rightCodeEditor, &CodeEditor::blockSelected, q, &DiffWidget::newCodeEditor_blockSelected);
    QObject::connect(q->leftCodeEditor->verticalScrollBar(), &QScrollBar::valueChanged, q, &DiffWidget::oldCodeEditor_scroll);
    QObject::connect(q->rightCodeEditor->verticalScrollBar(), &QScrollBar::valueChanged, q, &DiffWidget::newCodeEditor_scroll);
    QObject::connect(q->splitter, &QSplitter::splitterMoved, q, &DiffWidget::slotSplitterSplitterMoved);

    recalculateInfoPaneSize();

    mDefaultOption = q->leftCodeEditor->document()->defaultTextOption();

    QObject::connect(q->widgetSegmentsScrollBar, &SegmentsScrollBar::hover, q, &DiffWidget::slotSegmentsScrollbarHover);
    QObject::connect(q->widgetSegmentsScrollBar, &SegmentsScrollBar::mouseMove, q, &DiffWidget::slotSegmentsScrollbarMouseMove);
    QObject::connect(q->widgetSegmentsScrollBar, &SegmentsScrollBar::mouseEntered, mPreviewWidget, &QWidget::show);
    QObject::connect(q->widgetSegmentsScrollBar, &SegmentsScrollBar::mouseLeaved, mPreviewWidget, &QWidget::hide);
    q->showFilesInfo(true);
}

void DiffWidgetPrivate::createPreviewWidget()
{
    Q_Q(DiffWidget);

    mPreviewWidget = new QWidget(q);
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

    layout->setContentsMargins({});
    layout->setSpacing(q->segmentConnector->width() + 2 * (q->splitter->handleWidth()));

    mPreviewWidget->setStyleSheet(QStringLiteral("QPlainTextEdit{border: 3px solid gray;}"));
    mPreviewWidget->setLayout(layout);
    mPreviewWidget->hide();
}

void DiffWidgetPrivate::setEditorsContents(QList<Diff::DiffHunk *> segments)
{
    Q_Q(DiffWidget);

    q->leftCodeEditor->clearAll();
    q->rightCodeEditor->clearAll();

    mPreviewEditorLeft->clearAll();
    mPreviewEditorRight->clearAll();

    q->segmentConnector->setSegments(segments);
    q->segmentConnector->update();

    q->leftCodeEditor->setContent(lastDiff.left().lines, leftBlockDataList, mSameSize);
    q->rightCodeEditor->setContent(lastDiff.right().lines, rightBlockDataList, mSameSize);

    mPreviewEditorLeft->setContent(lastDiff.left().lines, leftBlockDataList, mSameSize);
    mPreviewEditorRight->setContent(lastDiff.right().lines, rightBlockDataList, mSameSize);

    q->scrollToTop();
}

#include "moc_diffwidget.cpp"
