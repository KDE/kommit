/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "searchdialog.h"
#include "fileviewerdialog.h"

#include <KLocalizedString>
#include <QStandardItemModel>
#include <QtConcurrent>
#include <entities/commit.h>
#include <gitmanager.h>

SearchDialog::SearchDialog(const QString &path, Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);
    initModel();
    lineEditPath->setText(path);

    connect(pushButtonSearch, &QPushButton::clicked, this, &SearchDialog::slotPushButtonSearchClicked);
    connect(treeView, &QTreeView::doubleClicked, this, &SearchDialog::slotTreeViewDoubleClicked);
}

void SearchDialog::initModel()
{
    if (!mModel) {
        mModel = new QStandardItemModel(this);
        treeView->setModel(mModel);
    }

    mModel->setColumnCount(3);
    mModel->setHeaderData(0, Qt::Horizontal, i18n("File name"));
    mModel->setHeaderData(1, Qt::Horizontal, i18n("Branch"));
    mModel->setHeaderData(2, Qt::Horizontal, i18n("Commit"));
}

SearchDialog::SearchDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);
    initModel();

    connect(pushButtonSearch, &QPushButton::clicked, this, &SearchDialog::slotPushButtonSearchClicked);
    connect(treeView, &QTreeView::doubleClicked, this, &SearchDialog::slotTreeViewDoubleClicked);
}

void SearchDialog::slotPushButtonSearchClicked()
{
    mModel->clear();
    initModel();
    startTimer(500);
    pushButtonSearch->setEnabled(false);
    mProgress.total = mProgress.value = 0;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    auto f = QtConcurrent::run(this, &SearchDialog::beginSearch);
#else
    auto f = QtConcurrent::run(&SearchDialog::beginSearch, this);
#endif
}

void SearchDialog::slotTreeViewDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    const auto file = mModel->data(mModel->index(index.row(), 0)).toString();
    const auto branch = mModel->data(mModel->index(index.row(), 1)).toString();
    const auto commit = mModel->data(mModel->index(index.row(), 2)).toString();

    QString place;
    if (!commit.isEmpty() && !branch.isEmpty())
        place = branch + QLatin1Char(':') + commit;
    else
        place = branch.isEmpty() ? commit : branch;

    auto d = new FileViewerDialog(mGit, place, file);
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->show();
}

void SearchDialog::beginSearch()
{
    if (radioButtonSearchBranches->isChecked()) {
        const auto branchesList = mGit->branches(Git::Manager::BranchType::LocalBranch);
        mProgress.total = branchesList.size();
        for (const auto &branch : branchesList) {
            searchOnPlace(branch, QString());
            mProgress.value++;
        }
    } else {
        Git::LogList list;
        list.load(mGit);

        mProgress.total = list.size();
        for (const auto &branch : std::as_const(list)) {
            searchOnPlace(QString(), branch->commitHash());
            mProgress.value++;
        }
    }

    pushButtonSearch->setEnabled(true);
}

void SearchDialog::searchOnPlace(const QString &branch, const QString &commit)
{
    const QString place = branch.isEmpty() ? commit : branch;
    const auto files = mGit->ls(place);

    for (const auto &file : std::as_const(files)) {
        if (!lineEditPath->text().isEmpty() && !file.contains(lineEditPath->text()))
            continue;

        bool ok = mGit->fileContent(place, file).contains(lineEditText->text(), checkBoxCaseSensetive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
        if (ok) {
            mModel->appendRow({new QStandardItem(file), new QStandardItem(branch), new QStandardItem(commit)});
        }
    }
}

void SearchDialog::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    progressBar->setMaximum(mProgress.total);
    progressBar->setValue(mProgress.value);
}

#include "moc_searchdialog.cpp"
