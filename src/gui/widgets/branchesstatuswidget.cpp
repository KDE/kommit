/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesstatuswidget.h"

#include "actions/branchactions.h"
#include "gitmanager.h"
#include "models/branchesmodel.h"
#include "core/kmessageboxhelper.h"

#include <KLocalizedString>
#include <QDebug>

BranchesStatusWidget::BranchesStatusWidget(QWidget *parent)
    : WidgetBase(parent)
{
    setupUi(this);
    init(Git::Manager::instance());
}

BranchesStatusWidget::BranchesStatusWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)

{
    setupUi(this);
    init(git);
}

void BranchesStatusWidget::init(Git::Manager *git)
{
    mActions = new BranchActions(git, this);
    mModel = git->branchesModel();
    treeView->setModel(mModel);

    comboBoxReferenceBranch->setModel(mModel);

    pushButtonNew->setAction(mActions->actionCreate());
    pushButtonBrowse->setAction(mActions->actionBrowse());
    pushButtonCheckout->setAction(mActions->actionCheckout());
    pushButtonDiff->setAction(mActions->actionDiff());
    pushButtonRemoveSelected->setAction(mActions->actionRemove());

    mActions->setOtherBranch(comboBoxReferenceBranch->currentText());
}

void BranchesStatusWidget::saveState(QSettings &settings) const
{
    save(settings, treeView);
}

void BranchesStatusWidget::restoreState(QSettings &settings)
{
    restore(settings, treeView);
}

void BranchesStatusWidget::on_comboBoxReferenceBranch_currentIndexChanged(const QString &selectedBranch)
{
    mModel->setReferenceBranch(selectedBranch);
    mActions->setOtherBranch(selectedBranch);
}

void BranchesStatusWidget::on_pushButtonRemoveSelected_clicked()
{
    if (!treeView->currentIndex().isValid())
        return;

    if (KMessageBoxHelper::removeQuestion(this, i18n("Are you sure to remove the selected branch?"))) {
        auto branchData = mGit->branchesModel()->fromIndex(treeView->currentIndex());
        if (branchData) {
            mGit->removeBranch(branchData->name);
            mGit->branchesModel()->load();
        }
    }
}

void BranchesStatusWidget::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto b = mModel->fromIndex(treeView->currentIndex());
    if (!b)
        return;
    mActions->setBranchName(b->name);
    mActions->popup();
}
