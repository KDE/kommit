/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diffdialog.h"

#include "models/difftreemodel.h"
#include "gitklient_appdebug.h"
#include <QFileDialog>
#include <git/gitmanager.h>

DiffDialog::DiffDialog(QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);
}

DiffDialog::DiffDialog(const Git::File &oldFile, const Git::File &newFile, QWidget *parent)
    : AppDialog(parent)
    , mOldFile(oldFile)
    , mNewFile(newFile)
{
    setupUi(this);
    diffWidget->setOldFile(oldFile);
    diffWidget->setNewFile(newFile);

    lineEditOldFileName->setText(oldFile.displayName());
    lineEditNewFileName->setText(newFile.displayName());

    diffWidget->compare();

    treeView->hide();
}

DiffDialog::DiffDialog(const QString &oldBranch, const QString &newBranch, QWidget *parent)
    : AppDialog(parent)
    , mOldBranch(oldBranch)
    , mNewBranch(newBranch)
{
    setupUi(this);

    mDiffModel = new DiffTreeModel;
    const auto diffs = Git::Manager::instance()->diffBranches(oldBranch, newBranch);

    for (const auto &f : diffs)
        mDiffModel->addFile(f);
    treeView->setModel(mDiffModel);
}

void DiffDialog::on_toolButtonShowHiddenChars_clicked(bool checked)
{
    diffWidget->showHiddenChars(checked);
}

void DiffDialog::on_pushButtonSaveAs_clicked()
{
    auto diff = Git::Manager::instance()->diff(mOldFile.fileName(), mNewFile.fileName());
    qCDebug(GITKLIENT_LOG).noquote() << diff;
    const auto fileName = QFileDialog::getSaveFileName(this, i18n("Save diff"));
    if (!fileName.isEmpty()) {
        QFile f(fileName);
        if (!f.open(QIODevice::WriteOnly)) {
            return;
        }
        f.write(diff.toUtf8());
        f.close();
    }
}

void DiffDialog::on_treeView_clicked(const QModelIndex &index)
{
    const auto fileName = mDiffModel->fullPath(index);

    Git::File oldFile(mOldBranch, fileName);
    Git::File newFile(mNewBranch, fileName);
    diffWidget->setOldFile(oldFile);
    diffWidget->setNewFile(newFile);
    diffWidget->compare();

    lineEditOldFileName->setText(oldFile.displayName());
    lineEditNewFileName->setText(newFile.displayName());
}
