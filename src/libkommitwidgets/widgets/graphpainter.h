/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "gitgraphlane.h"
#include <QStyledItemDelegate>

#include "libkommitwidgets_export.h"

class CommitsModel;
class LIBKOMMITWIDGETS_EXPORT GraphPainter : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit GraphPainter(CommitsModel *model, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paintLane(QPainter *painter, const GraphLane &lane, int index) const;
    int colX(int col) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    CommitsModel *const mModel;
    QVector<QColor> mColors;
};
