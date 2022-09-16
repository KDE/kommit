/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "git/gitgraphlane.h"
#include <QStyledItemDelegate>

namespace Git
{
class LogsModel;
}
class GraphPainter : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit GraphPainter(Git::LogsModel *model, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paintLane(QPainter *painter, const Git::GraphLane &lane, int index) const;
    int colX(const int &col) const;

    // QAbstractItemDelegate interface
public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    Git::LogsModel *mModel;
    QVector<QColor> mColors;
};
