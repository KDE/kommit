/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QStyledItemDelegate>

#include "libkommitwidgets_export.h"

class GraphPainterPrivate;
class CommitsModel;
class LIBKOMMITWIDGETS_EXPORT GraphPainter : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit GraphPainter(CommitsModel *model, QObject *parent = nullptr);
    virtual ~GraphPainter();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    GraphPainterPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(GraphPainter)
};
