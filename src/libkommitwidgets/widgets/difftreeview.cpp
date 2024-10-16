/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "difftreeview.h"
#include "models/difftreemodel.h"
#include "models/filesmodel.h"

#include <QKeyEvent>
#include <QSortFilterProxyModel>

DiffTreeModel *DiffTreeView::diffModel() const
{
    return mDiffModel;
}

void DiffTreeView::setModels(DiffTreeModel *newDiffModel, FilesModel *filesModel)
{
    mDiffModel = newDiffModel;
    mFilesModel = filesModel;

    mFilterModel->setSourceModel(filesModel);
    listView->setModel(mFilterModel);

    treeView->setModel(mDiffModel);
}

DiffTreeView::DiffTreeView(QWidget *parent)
    : QWidget(parent)
    , mFilterModel(new QSortFilterProxyModel(this))
{
    setupUi(this);
    mFilterModel->setFilterKeyColumn(0);
    mFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    listView->setModel(mFilterModel);

    connect(checkBoxHideUnchangeds, &QAbstractButton::toggled, this, &DiffTreeView::hideUnchangedsChanged);
    checkBoxHideUnchangeds->hide();

    lineEditFilter->installEventFilter(this);
    listView->installEventFilter(this);

    connect(lineEditFilter, &QLineEdit::textEdited, this, &DiffTreeView::lineEditFilterTextChanged);
    connect(treeView, &QTreeView::clicked, this, &DiffTreeView::slotTreeViewClicked);
    connect(listView, &QListView::clicked, this, &DiffTreeView::slotListViewClicked);
}

void DiffTreeView::lineEditFilterTextChanged(const QString &text)
{
    stackedWidget->setCurrentIndex(text.isEmpty() ? 0 : 1);
    mFilterModel->setFilterRegularExpression(QStringLiteral(".*") + text + QStringLiteral(".*"));
}

void DiffTreeView::slotTreeViewClicked(const QModelIndex &index)
{
    if (!mDiffModel->rowCount(index)) {
        const auto fileName = mDiffModel->fullPath(index);
        Q_EMIT fileSelected(fileName);
    }
}

void DiffTreeView::slotListViewClicked(const QModelIndex &index)
{
    const auto row = mFilterModel->mapToSource(index).row();
    const auto fileName = mFilesModel->data(mFilesModel->index(row, 1), Qt::DisplayRole);
    Q_EMIT fileSelected(fileName.toString());
}

bool DiffTreeView::hideUnchangeds() const
{
    return checkBoxHideUnchangeds->isChecked();
}

void DiffTreeView::setHideUnchangeds(bool newHideUnchangeds)
{
    if (checkBoxHideUnchangeds->isChecked() == newHideUnchangeds)
        return;
    checkBoxHideUnchangeds->setChecked(newHideUnchangeds);
    Q_EMIT hideUnchangedsChanged();
}

bool DiffTreeView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lineEditFilter && event->type() == QEvent::KeyPress) {
        auto ke = static_cast<QKeyEvent *>(event);

        if (ke->key() == Qt::Key_Down) {
            if (stackedWidget->currentIndex() == 0)
                treeView->setFocus();
            else
                listView->setFocus();
        }
    }
    if (watched == listView && event->type() == QEvent::KeyPress) {
        auto ke = static_cast<QKeyEvent *>(event);

        if (ke->key() == Qt::Key_Up && listView->currentIndex().row() == 0)
            lineEditFilter->setFocus();
    }
    return QWidget::eventFilter(watched, event);
}

#include "moc_difftreeview.cpp"
