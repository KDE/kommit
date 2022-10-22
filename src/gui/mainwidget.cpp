/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mainwidget.h"
#include "buttonsgroup.h"
#include "dialogs/taginfodialog.h"
#include "diffwindow.h"
#include "models/treemodel.h"

#include "gitklient_appdebug.h"
#include <QMenu>
#include <QToolButton>

#include "dialogs/filestreedialog.h"
#include "git/gitmanager.h"
#include "git/gitremote.h"

Git::Manager *MainWidget::git() const
{
    return _git;
}

void MainWidget::setGit(Git::Manager *newGit)
{
    _git = newGit;
}

void MainWidget::reload()
{
    _branches = _git->branches();

    _repoModel->clear();
    _repoModel->addData(_git->branches());
    treeViewRepo->setModel(_repoModel);

    listWidgetTags->clear();
    listWidgetTags->addItems(_git->tags());
}

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    auto actionBrowseBranch = new QAction(this);
    auto actionDiffBranch = new QAction(this);
    Q_SET_OBJECT_NAME(actionBrowseBranch);
    Q_SET_OBJECT_NAME(actionDiffBranch);

    setupUi(this);
    _repoModel = new TreeModel(this);

    _git = Git::Manager::instance();
    connect(_git, &Git::Manager::pathChanged, this, &MainWidget::reload);

    auto styleSheet = QString(R"CSS(
        #scrollAreaWidgetContents {
            background-color: #%1;
        }
        QToolButton {
            background-color: #%1;
            border: none;
            padding-top: 10px;
            padding-bottom: 10px;
            height: 48px;
        }

        QToolButton:hover {
            background-color: #%2;
        }

        QToolButton:checked {
            background-color: #%3;
            color: white;
        }

)CSS")
                          .arg(palette().color(QPalette::Base).rgba(), 0, 16)
                          .arg(palette().color(QPalette::Highlight).lighter().rgba(), 0, 16)
                          .arg(palette().color(QPalette::Highlight).rgba(), 0, 16);

    scrollAreaWidgetContents->setStyleSheet(styleSheet);

    _pagesButtonsGroup = new ButtonsGroup(this);
    _pagesButtonsGroup->addButton(toolButtonOverview);
    _pagesButtonsGroup->addButton(toolButtonBranches);
    _pagesButtonsGroup->addButton(toolButtonCommits);
    _pagesButtonsGroup->addButton(toolButtonStashes);
    _pagesButtonsGroup->addButton(toolButtonRemotes);
    _pagesButtonsGroup->addButton(toolButtonTags);
    _pagesButtonsGroup->addButton(toolButtonSubmodules);
    connect(_pagesButtonsGroup, &ButtonsGroup::clicked, this, &MainWidget::pageButtons_clicked);
    _pagesButtonsGroup->simulateClickOn(0);

    _branchesMenu = new QMenu(this);
    _branchesMenu->addAction(actionBrowseBranch);
    _branchesMenu->addAction(actionDiffBranch);

    actionBrowseBranch->setText(i18n("Browse..."));
    actionDiffBranch->setText(i18n("Diff with master..."));

    connect(treeViewRepo, &QTreeView::activated, this, &::slotTreeViewRepoActivated);
    connect(treeViewRepo, &QTreeViewRepo::customContextMenuRequested, this, &::slotTreeViewRepoCustomContextMenuRequested);
    connect(actionBrowseBranch, &QAction::triggered, this, &::slotActionBrowseBranchTriggered);
    connect(actionDiffBranch, &QAction::triggered, this, &::slotActionDiffBranchTriggered);
    connect(pushButtonAddTag, &QPushButton::clicked, this, &::slotPushButtonAddTagClicked);
}

MainWidget::~MainWidget()
{
}

void MainWidget::listButton_clicked()
{
    auto btn = qobject_cast<QToolButton *>(sender());
    if (!btn)
        return;

    int index = btn->property("listIndex").toInt();
    qCDebug(GITKLIENT_LOG) << index << "clicked";
    btn->setChecked(true);
    stackedWidget->setCurrentIndex(index);
}

void MainWidget::pageButtons_clicked(int index)
{
    auto btn = _pagesButtonsGroup->at(index);
    labelTitle->setText(btn->text().replace("&", ""));
    labelPageIcon->setPixmap(btn->icon().pixmap({32, 32}));
    stackedWidget->setCurrentIndex(index);
    return;
}

void MainWidget::slotTreeViewRepoActivated(const QModelIndex &index)
{
    auto key = _repoModel->key(index);
    if (!key.isEmpty())
        widgetCommitsView->setBranch(key);
}

void MainWidget::slotTreeViewRepoCustomContextMenuRequested(const QPoint &pos)
{
    _branchesMenu->popup(treeViewRepo->mapToGlobal(pos));
}

void MainWidget::slotActionBrowseBranchTriggered()
{
    auto branchName = _repoModel->fullPath(treeViewRepo->currentIndex());
    FilesTreeDialog d(branchName, this);
    d.exec();
}

void MainWidget::slotActionDiffBranchTriggered()
{
    auto branchName = _repoModel->fullPath(treeViewRepo->currentIndex());

    auto diffWin = new DiffWindow(branchName, "master");
    diffWin->showModal();
}

void MainWidget::slotPushButtonAddTagClicked()
{
    TagInfoDialog d(this);
    d.setWindowTitle(i18n("New tag"));
    if (d.exec() == QDialog::Accepted) {
        _git->createTag(d.tagName(), d.message());
    }
}
