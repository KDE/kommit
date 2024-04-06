/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diffwindow.h"

#include <KActionCollection>
#include <KLocalizedString>

#include <QDebug>
#include <QDockWidget>
#include <QTreeView>

#include <core/editactionsmapper.h>
#include <dialogs/diffopendialog.h>
#include <entities/branch.h>
#include <entities/tag.h>
#include <entities/tree.h>
#include <gitmanager.h>
#include <models/difftreemodel.h>
#include <models/filesmodel.h>
#include <widgets/difftreeview.h>
#include <widgets/diffwidget.h>

DiffWindow::DiffWindow()
    : AppMainWindow()
{
    init(true);
}

DiffWindow::DiffWindow(Git::Manager *git)
    : AppMainWindow()
    , mGit(git)
{
    init(true);

    mOldBranch = git->currentBranch();
    const auto diffs = git->diffBranch(mOldBranch);

    for (const auto &f : diffs) {
        mDiffModel->addFile(f);
        mFilesModel->append(f.name());
    }

    mLeftStorage.setGitBranch(git, git->currentBranch());
    mRightStorage.setPath(git->path());
    mDiffModel->sortItems();
}

DiffWindow::DiffWindow(QSharedPointer<Git::File> oldFile, QSharedPointer<Git::File> newFile)
    : AppMainWindow()
    , mOldFile(oldFile)
    , mNewFile(newFile)
{
    init(false);

    mDiffWidget->setOldFile(oldFile);
    mDiffWidget->setNewFile(newFile);
    mDiffWidget->compare();
}

DiffWindow::DiffWindow(Git::Manager *git, const QString &oldBranch, const QString &newBranch)
    : AppMainWindow()
    , mGit(git)
    , mOldBranch(oldBranch)
    , mNewBranch(newBranch)
{
    init(true);

    mLeftStorage.setGitBranch(git, oldBranch);
    mRightStorage.setGitBranch(git, newBranch);

    const auto diffs = git->diffBranches(oldBranch, newBranch);

    for (auto &f : diffs) {
        mDiffModel->addFile(f);
        //        qCDebug(KOMMIT_LOG) << f.name() << f.status();
        mFilesModel->append(f.name());
    }
    // mLeftStorage.set = mRightStorage = Git;
    mDiffModel->sortItems();
}

DiffWindow::DiffWindow(Git::Manager *git, QSharedPointer<Git::Tag> tag)
    : AppMainWindow()
    , mOldBranch(tag->name())
    , mNewBranch("Working dir")
{
    init(true);

    auto tree = tag->commit()->tree();
    const auto diffs = git->diff(tree);

    for (auto &f : diffs) {
        mDiffModel->addFile(f);
        //        qCDebug(KOMMIT_LOG) << f.name() << f.status();
        mFilesModel->append(f.newFile());
    }
    mLeftStorage.setTree(tree);
    mRightStorage.setPath(git->path());
    mDiffModel->sortItems();
}

DiffWindow::DiffWindow(Git::Branch *oldBranch, Git::Branch *newBranch)
{
    mLeftStorage.setTree(oldBranch->tree());
    mRightStorage.setTree(newBranch->tree());
}

DiffWindow::DiffWindow(QSharedPointer<Git::Branch> oldBranch, QSharedPointer<Git::Branch> newBranch)
    : AppMainWindow()
{
    init(true);
    mLeftStorage.setTree(oldBranch->tree());
    mRightStorage.setTree(newBranch->tree());
}

DiffWindow::DiffWindow(Git::Manager *git, QSharedPointer<Git::Tree> leftTree)
{
    init(true);
    mLeftStorage.setTree(leftTree);
    mRightStorage.setPath(git->path());

    const auto diffs = git->diff(leftTree);

    for (auto &f : diffs) {
        mDiffModel->addFile(f);
        //        qCDebug(KOMMIT_LOG) << f.name() << f.status();
        mFilesModel->append(f.newFile());
    }

    mDiffModel->sortItems();
}

DiffWindow::DiffWindow(const QString &oldDir, const QString &newDir)
{
    init(true);

    mLeftDir = oldDir;
    mRightDir = newDir;
    compareDirs();

    mLeftStorage.setPath(oldDir);
    mRightStorage.setPath(newDir);
}

void DiffWindow::init(bool showSideBar)
{
    auto mapper = new EditActionsMapper(this);
    mDiffWidget = new DiffWidget(this);
    mDiffWidget->showSameSize(true);

    mapper->init(actionCollection());

    setCentralWidget(mDiffWidget);

    mapper->addTextEdit(mDiffWidget->oldCodeEditor());
    mapper->addTextEdit(mDiffWidget->newCodeEditor());
    setWindowTitle(i18nc("@title:window", "Kommit Diff[*]"));

    mDock = new QDockWidget(this);
    mDock->setWindowTitle(i18nc("@title:window", "Tree"));
    mDock->setObjectName(QStringLiteral("treeViewDock"));

    mTreeView = new DiffTreeView(this);
    connect(mTreeView, &DiffTreeView::fileSelected, this, &DiffWindow::slotTreeViewFileSelected);
    mDock->setWidget(mTreeView);
    mDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea, mDock);

    mFilesModel = new FilesModel(this);
    mDiffModel = new DiffTreeModel(this);
    mDiffModel->setShowRoot(true);
    //_treeView->setDiffModel(_diffModel, _filesModel);

    initActions();
    setupGUI(StandardWindowOption::Default, QStringLiteral("kommitdiffui.rc"));

    mDock->setVisible(showSideBar);

    mTreeView->setModels(mDiffModel, mFilesModel);
}

void DiffWindow::initActions()
{
    auto actionCollection = this->actionCollection();

    auto viewHiddenCharsAction = actionCollection->addAction(QStringLiteral("view_hidden_chars"));
    viewHiddenCharsAction->setText(i18n("View hidden chars..."));
    viewHiddenCharsAction->setCheckable(true);
    connect(viewHiddenCharsAction, &QAction::triggered, mDiffWidget, &DiffWidget::showHiddenChars);

    auto viewSameSizeBlocksAction = actionCollection->addAction(QStringLiteral("view_same_size_blocks"), mDiffWidget, &DiffWidget::showSameSize);
    viewSameSizeBlocksAction->setText(i18n("Same size blocks"));
    viewSameSizeBlocksAction->setCheckable(true);
    viewSameSizeBlocksAction->setChecked(true);

    auto viewFilesInfo = actionCollection->addAction(QStringLiteral("view_files_info"), mDiffWidget, &DiffWidget::showFilesInfo);
    viewFilesInfo->setText(i18n("Show files names"));
    viewFilesInfo->setCheckable(true);
    viewFilesInfo->setChecked(true);

    auto showTreeDockAction = mDock->toggleViewAction();
    actionCollection->addAction(QStringLiteral("show_tree_dock"), showTreeDockAction);
    showTreeDockAction->setText(i18n("Show Tree"));

    auto reloadAction = actionCollection->addAction(QStringLiteral("file_reload"), mDiffWidget, &DiffWidget::compare);
    reloadAction->setText(i18n("Reload"));

    KStandardAction::quit(this, &QWidget::close, actionCollection);
    KStandardAction::open(this, &DiffWindow::fileOpen, actionCollection);

#ifdef UNDEF
    auto settingsManager = new SettingsManager(mGit, this);
    KStandardAction::preferences(settingsManager, &SettingsManager::show, actionCollection);
#endif
}

void DiffWindow::fileOpen()
{
    DiffOpenDialog d(this);
    if (d.exec() != QDialog::Accepted)
        return;

    if (d.mode() == DiffOpenDialog::Dirs) {
        mLeftStorage.setPath(d.oldDir());
        mRightStorage.setPath(d.newDir());
        compareDirs();
    } else {
        mDiffWidget->setOldFile(QSharedPointer<Git::File>{new Git::File{d.oldFile()}});
        mDiffWidget->setNewFile(QSharedPointer<Git::File>{new Git::File{d.newFile()}});
        mDiffWidget->compare();
    }
}

void DiffWindow::slotTreeViewFileSelected(const QString &file)
{
    // switch (mLeftStorage) {
    // case FileSystem:
    //     mDiffWidget->setOldFile(Git::File{mLeftDir + QLatin1Char('/') + file});
    //     break;
    // case Git:
    //     mDiffWidget->setOldFile({mGit, mOldBranch, file});
    //     break;
    // case NoStorage:
    //     return;
    // }
    // switch (mRightStorage) {
    // case FileSystem:
    //     mDiffWidget->setNewFile(Git::File{mRightDir + QLatin1Char('/') + file});
    //     break;
    // case Git:
    //     mDiffWidget->setNewFile({mGit, mNewBranch, file});
    //     break;
    // case NoStorage:
    //     return;
    // }

    mDiffWidget->setOldFile(mLeftStorage.file(file));
    mDiffWidget->setNewFile(mRightStorage.file(file));

    mDiffWidget->compare();
}

QString diffTypeText(const Diff::DiffType type)
{
    switch (type) {
    case Diff::DiffType::Unchanged:
        return i18n("Unchanged");
    case Diff::DiffType::Added:
        return i18n("Added");
    case Diff::DiffType::Removed:
        return i18n("Removed");
    case Diff::DiffType::Modified:
        return i18n("Modified");
    }
    return {};
}

void DiffWindow::compareDirs()
{
    auto map = Diff::diffDirs(mLeftDir, mRightDir);
    for (auto i = map.begin(); i != map.end(); ++i) {
        mDiffModel->addFile(i.key(), i.value());
    }
    mDiffModel->sortItems();

    mDock->show();
}

QSharedPointer<Git::File> DiffWindow::Storage::file(const QString &path) const
{
    switch (mMode) {
    case Mode::NoStorage:
        break;
    case Mode::FileSystem:
        return QSharedPointer<Git::File>{new Git::File{mPath + path}};
    case Mode::Git:
        return QSharedPointer<Git::File>{new Git::File{mManager, mBranchName, path}};
    case Mode::Tree:
        return mTree->file(path);
    }

    return {};
}

void DiffWindow::Storage::setGitBranch(Git::Manager *manager, const QString &branchName)
{
    mMode = Mode::Git;
    mManager = manager;
    mBranchName = branchName;
}

void DiffWindow::Storage::setPath(const QString &path)
{
    mPath = path;
    if (!mPath.endsWith("/"))
        mPath += "/";
    mMode = Mode::FileSystem;
}

void DiffWindow::Storage::setTree(QSharedPointer<Git::Tree> tree)
{
    mTree = tree;
    mMode = Mode::Tree;
}
#include "moc_diffwindow.cpp"
