/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef SEGMENTSSCROLLBAR_H
#define SEGMENTSSCROLLBAR_H

#include <QWidget>

class SegmentConnector;
class SegmentsScrollBar : public QWidget
{
    Q_OBJECT
public:
    explicit SegmentsScrollBar(QWidget *parent = nullptr);

    SegmentConnector *segmentConnector() const;
    void setSegmentConnector(SegmentConnector *newSegmentConnector);

protected:
    void paintEvent(QPaintEvent *event) override;

private Q_SLOTS:
    void reload();

private:
    enum Side {
        Left,
        Right
    };

    int leftCount{0};
    int rightCount{0};
    SegmentConnector *mSegmentConnector{nullptr};

    inline void paintSection(QPainter &painter,
                             const Side &side,
                             int from,
                             int len,
                             const QBrush &brush,
                             bool drawRect = false);
};

#endif // SEGMENTSSCROLLBAR_H
