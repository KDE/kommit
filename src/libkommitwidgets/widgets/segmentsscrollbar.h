/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QWidget>

#include "libkommitwidgets_export.h"

#include <QScopedPointer>

class SegmentConnector;
class SegmentsScrollBarPrivate;

class LIBKOMMITWIDGETS_EXPORT SegmentsScrollBar : public QWidget
{
    Q_OBJECT
public:
    explicit SegmentsScrollBar(QWidget *parent = nullptr);

    SegmentConnector *segmentConnector() const;
    void setSegmentConnector(SegmentConnector *newSegmentConnector);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEvent *event) override;
#else
    void enterEvent(QEnterEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;

private Q_SLOTS:
    void reload();

Q_SIGNALS:
    void hover(int y, double pos);
    void mouseMove(int y, double pos);
    void mouseEntered();
    void mouseLeaved();

private:
    QScopedPointer<SegmentsScrollBarPrivate> d_ptr;
    Q_DECLARE_PRIVATE(SegmentsScrollBar)
};
