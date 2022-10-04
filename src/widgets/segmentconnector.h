/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../diff/diff.h"
#include <QMap>
#include <QWidget>

class CodeEditor;
class SegmentConnector : public QWidget
{
    Q_OBJECT

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

    Q_REQUIRED_RESULT bool sameSize() const;
    void setSameSize(bool newSameSize);

    Q_REQUIRED_RESULT int topMargin() const;
    void setTopMargin(int newTopMargin);

Q_SIGNALS:
    void sameSizeChanged();
    void segmentsChanged();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *mLeft{nullptr};
    CodeEditor *mRight{nullptr};
    QList<Diff::Segment *> mSegments;
    Diff::Segment *mCurrentSegment{nullptr};
    bool m_sameSize{false};
    int m_topMargin = 0;
    struct SegmentPos {
        int leftStart;
        int leftEnd;
        int rightStart;
        int rightEnd;
    };

    QMap<Diff::Segment *, SegmentPos> mSegmentPos;
};
