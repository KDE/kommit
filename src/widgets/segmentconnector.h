/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../diff/diff.h"
#include <QtCore/QMap>
#include <QtWidgets/QWidget>

class CodeEditor;
class SegmentConnector : public QWidget
{
    Q_OBJECT
    CodeEditor *_left{nullptr};
    CodeEditor *_right{nullptr};
    QList<Diff::Segment *> _segments;
    Diff::Segment *_currentSegment{nullptr};
    bool m_sameSize{false};
    struct SegmentPos {
        int leftStart;
        int leftEnd;
        int rightStart;
        int rightEnd;
    };

    QMap<Diff::Segment *, SegmentPos> _segmentPos;

public:
    explicit SegmentConnector(QWidget *parent = nullptr);

    CodeEditor *left() const;
    void setLeft(CodeEditor *newLeft);

    CodeEditor *right() const;
    void setRight(CodeEditor *newRight);

    const QList<Diff::Segment *> &segments() const;
    void setSegments(const QList<Diff::Segment *> &newSegments);

    Diff::Segment *currentSegment() const;
    void setCurrentSegment(Diff::Segment *newCurrentSegment);

    bool sameSize() const;
    void setSameSize(bool newSameSize);

Q_SIGNALS:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};
