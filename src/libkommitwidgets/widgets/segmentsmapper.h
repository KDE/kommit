/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMap>
#include <QObject>
#include <diff.h>

#include "libkommitwidgets_export.h"

class SegmentsMapperPrivate;
class CodeEditor;
class LIBKOMMITWIDGETS_EXPORT SegmentsMapper : public QObject
{
    Q_OBJECT

public:
    explicit SegmentsMapper(QObject *parent = nullptr);
    ~SegmentsMapper();

    void addEditor(CodeEditor *editor);
    const QList<Diff::Segment *> &segments() const;
    void setSegments(const QList<Diff::MergeSegment *> &newSegments);

    int map(int from, int to, int index) const;

    Diff::Segment *currentSegment() const;
    void refresh();

    void setCurrentSegment(Diff::Segment *newCurrentSegment);
    Q_REQUIRED_RESULT bool isMergeable() const;

    Q_REQUIRED_RESULT int conflicts() const;

    void findPrevious(Diff::SegmentType type);
    void findNext(Diff::SegmentType type);

private:
    SegmentsMapperPrivate *d_ptr;
    Q_DECLARE_PRIVATE(SegmentsMapper)

    LIBKOMMITWIDGETS_NO_EXPORT void codeEditor_blockSelected();
    LIBKOMMITWIDGETS_NO_EXPORT void codeEditor_scroll(int value);
};
