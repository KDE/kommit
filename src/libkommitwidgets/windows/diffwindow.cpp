/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diffwindow.h"
#include "caches/branchescache.h"

#include <KActionCollection>
#include <KLocalizedString>

#include <QDockWidget>
#include <QTreeView>

#include <core/editactionsmapper.h>
#include <dialogs/diffopendialog.h>
#include <entities/branch.h>
#include <entities/tag.h>
#include <entities/tree.h>
#include <models/difftreemodel.h>
#include <models/filesmodel.h>
#include <repository.h>
#include <widgets/difftreeview.h>
#include <widgets/diffwidget.h>

namespace Impl
{

class Storage
{
public:
    enum class Mode {
        NotSet,
        File,
        Blob,
        Tree,
        Dir
    };

    Mode _mode{Mode::NotSet};
    QString _path;
    QString _title;
    QString _filePath;

    Git::Tree _tree;
    Git::Blob _blob;

    void setFile(const QString &path)
    {
        _mode = Mode::File;
        _filePath = path;
    }
    void setFile(Git::Blob file)
    {
        _mode = Mode::Blob;
        _blob = file;
    }

    void setTree(Git::ITree *tree)
    {
        _title = tree->treeTitle();
        _tree = tree->tree();
        _mode = Mode::Tree;
    }
    void setDir(const QString &path)
    {
        _mode = Mode::Dir;
        _path = path;
    }

    QString title(const QString &file) const
    {
        switch (_mode) {
        case Mode::File:
            return _filePath;

        case Mode::Blob:
            return _blob.name();

        case Mode::Tree:
            return _title + QStringLiteral(":") + file;

        case Mode::Dir:
            return _path + QStringLiteral(":") + file;
        }
        return {};
    }

    QByteArray content(const QString &file) const
    {
        switch (_mode) {
        case Mode::Blob:
            return _blob.content();

        case Mode::File: {
            QFile f{_filePath};
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
                return {};
            auto buffer = f.readAll();
            f.close();
            return buffer;
        }
        case Mode::Dir: {
            QFile f{_path + file};
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
                return {};
            auto buffer = f.readAll();
            f.close();
            return buffer;
        }
        case Mode::Tree: {
            auto f = _tree.file(file);
            if (f.isNull())
                return {};
            return f.content();
        }
        }

        return {};
    }
};

}

class DiffWindowPrivate
{
    DiffWindow *const q_ptr;
    Q_DECLARE_PUBLIC(DiffWindow)

public:
    enum class Mode {
        None,
        Dirs,
        Files
    };
    explicit DiffWindowPrivate(DiffWindow *parent);

    EditActionsMapper *mapper{nullptr};
    DiffWidget *diffWidget{nullptr};
    FilesModel *filesModel{nullptr};
    DiffTreeModel *diffModel{nullptr};
    DiffTreeView *treeView{nullptr};
    QDockWidget *dock{nullptr};

    Git::Repository *manager{nullptr};
    Impl::Storage left;
    Impl::Storage right;

    Mode mode{Mode::None};

    void setFiles(const QString &file);

    void compareFile(const QString &file);

    void compareDirs();
    void initActions();

    void init(bool showSideBar);
};

DiffWindowPrivate::DiffWindowPrivate(DiffWindow *parent)
    : q_ptr{parent}
    , mapper{new EditActionsMapper(parent)}
    , left{}
    , right{}
    , diffWidget{new DiffWidget(parent)}
    , filesModel{new FilesModel(parent)}
    , diffModel{new DiffTreeModel(parent)}
    , treeView{new DiffTreeView(parent)}
    , dock{new QDockWidget(parent)}
{
}

void DiffWindowPrivate::setFiles(const QString &file)
{
    left.setFile(file);
    right.setFile(file);
}

void DiffWindowPrivate::compareFile(const QString &file)
{
    diffWidget->setOldFile(left.title(file), left.content(file));
    diffWidget->setNewFile(right.title(file), right.content(file));
    diffWidget->compare();
}

void DiffWindowPrivate::compareDirs()
{
    if (left._mode == Impl::Storage::Mode::Tree) {
        auto diff = manager->diff(left._tree, right._tree);

        for (auto &d : diff) {
            Diff::DiffType type{Diff::DiffType::Unchanged};

            switch (d.status()) {
            case Git::ChangeStatus::Unknown:
            case Git::ChangeStatus::Unmodified:
            case Git::ChangeStatus::Ignored:
            case Git::ChangeStatus::Untracked:
            case Git::ChangeStatus::Unreadable:
                type = Diff::DiffType::Unchanged;
                break;
            case Git::ChangeStatus::Modified:
            case Git::ChangeStatus::Renamed:
            case Git::ChangeStatus::Copied:
            case Git::ChangeStatus::TypeChange:
            case Git::ChangeStatus::UpdatedButInmerged:
            case Git::ChangeStatus::Conflicted:
                type = Diff::DiffType::Modified;
                break;
            case Git::ChangeStatus::Added:
                type = Diff::DiffType::Added;
                break;
            case Git::ChangeStatus::Removed:
                type = Diff::DiffType::Removed;
                break;
            }

            diffModel->addFile(d.newFile(), type);
        }
    } else {
        auto map = Diff::diffDirs(left._path, right._path);
        diffModel->clear();
        for (auto i = map.begin(); i != map.end(); ++i) {
            diffModel->addFile(i.key(), i.value());
        }
    }
    diffModel->sortItems();
    dock->show();
}

void DiffWindowPrivate::initActions()
{
    Q_Q(DiffWindow);
    auto actionCollection = q->actionCollection();

    auto viewHiddenCharsAction = actionCollection->addAction(QStringLiteral("view_hidden_chars"));
    viewHiddenCharsAction->setText(i18n("View hidden chars…"));
    viewHiddenCharsAction->setCheckable(true);
    QObject::connect(viewHiddenCharsAction, &QAction::triggered, diffWidget, &DiffWidget::showHiddenChars);

    auto viewSameSizeBlocksAction = actionCollection->addAction(QStringLiteral("view_same_size_blocks"), diffWidget, &DiffWidget::showSameSize);
    viewSameSizeBlocksAction->setText(i18n("Same size blocks"));
    viewSameSizeBlocksAction->setCheckable(true);
    viewSameSizeBlocksAction->setChecked(true);

    auto viewFilesInfo = actionCollection->addAction(QStringLiteral("view_files_info"), diffWidget, &DiffWidget::showFilesInfo);
    viewFilesInfo->setText(i18n("Show files names"));
    viewFilesInfo->setCheckable(true);
    viewFilesInfo->setChecked(true);

    auto showTreeDockAction = dock->toggleViewAction();
    actionCollection->addAction(QStringLiteral("show_tree_dock"), showTreeDockAction);
    showTreeDockAction->setText(i18n("Show Tree"));

    auto reloadAction = actionCollection->addAction(QStringLiteral("file_reload"), diffWidget, &DiffWidget::compare);
    reloadAction->setText(i18n("Reload"));

    KStandardAction::quit(q, &QWidget::close, actionCollection);
    KStandardAction::open(q, &DiffWindow::fileOpen, actionCollection);

#ifdef UNDEF
    auto settingsManager = new SettingsManager(mGit, this);
    KStandardAction::preferences(settingsManager, &SettingsManager::show, actionCollection);
#endif
}

void DiffWindowPrivate::init(bool showSideBar)
{
    Q_Q(DiffWindow);

    diffWidget->showSameSize(q);

    mapper->init(q->actionCollection());

    q->setCentralWidget(diffWidget);

    mapper->addTextEdit(diffWidget->oldCodeEditor());
    mapper->addTextEdit(diffWidget->newCodeEditor());
    q->setWindowTitle(i18nc("@title:window", "Kommit Diff[*]"));

    dock->setWindowTitle(i18nc("@title:window", "Tree"));
    dock->setObjectName(QStringLiteral("treeViewDock"));

    QObject::connect(treeView, &DiffTreeView::fileSelected, q, &DiffWindow::slotTreeViewFileSelected);
    dock->setWidget(treeView);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    q->addDockWidget(Qt::LeftDockWidgetArea, dock);

    initActions();
    q->setupGUI(KXmlGuiWindow::StandardWindowOption::Default, QStringLiteral("kommitdiffui.rc"));

    dock->setVisible(showSideBar);
    treeView->setModels(diffModel, filesModel);
}

DiffWindow::DiffWindow()
    : AppMainWindow()
    , d_ptr{new DiffWindowPrivate{this}}
{
    Q_D(DiffWindow);
    d->init(false);
}

DiffWindow::DiffWindow(const Git::Blob &oldFile, const Git::Blob &newFile)
    : AppMainWindow()
    , d_ptr{new DiffWindowPrivate{this}}
{
    Q_D(DiffWindow);

    d->init(false);
    d->left.setFile(oldFile);
    d->right.setFile(newFile);
    d->compareFile("");
}

DiffWindow::DiffWindow(Git::Repository *git, Git::ITree *leftTree)
    : AppMainWindow()
    , d_ptr{new DiffWindowPrivate{this}}
{
    Q_D(DiffWindow);

    d->init(true);
    d->manager = git;
    d->left.setTree(leftTree);
    d->right.setDir(git->path());
    d->compareDirs();
}

DiffWindow::DiffWindow(Git::Repository *git, Git::ITree *leftTree, Git::ITree *rightTree)
    : AppMainWindow()
    , d_ptr{new DiffWindowPrivate{this}}
{
    Q_D(DiffWindow);

    d->init(true);
    d->manager = git;
    d->left.setTree(leftTree);
    d->right.setTree(rightTree);
    d->compareDirs();
}

DiffWindow::DiffWindow(Mode mode, const QString &left, const QString &right)
    : AppMainWindow()
    , d_ptr{new DiffWindowPrivate{this}}
{
    Q_D(DiffWindow);

    switch (mode) {
    case DiffWindow::Files:
        d->init(true);
        d->left.setFile(left);
        d->right.setFile(right);
        d->compareFile("");
        break;
    case DiffWindow::Dirs:
        d->init(true);
        d->left.setDir(left);
        d->right.setDir(right);
        d->compareDirs();
        break;
    }
}

DiffWindow::~DiffWindow()
{
}

void DiffWindow::setLeft(const QString &filePath)
{
    Q_D(DiffWindow);
    d->left.setFile(filePath);
}

void DiffWindow::setLeft(Git::Blob blob)
{
    Q_D(DiffWindow);
    d->left.setFile(blob);
}

void DiffWindow::setRight(const QString &filePath)
{
    Q_D(DiffWindow);
    d->right.setFile(filePath);
}

void DiffWindow::setRight(Git::Blob blob)
{
    Q_D(DiffWindow);
    d->right.setFile(blob);
}

void DiffWindow::compare()
{
    Q_D(DiffWindow);
    d->compareFile("");
}

void DiffWindow::fileOpen()
{
    Q_D(DiffWindow);
    DiffOpenDialog openDialog(this);
    if (openDialog.exec() != QDialog::Accepted)
        return;

    if (openDialog.mode() == DiffOpenDialog::Dirs) {
        d->left.setDir(openDialog.oldDir());
        d->right.setDir(openDialog.newDir());
        d->compareDirs();
    } else {
        d->left.setFile(openDialog.oldFile());
        d->right.setFile(openDialog.newFile());
        d->compareFile("");
    }
    d->dock->setVisible(openDialog.mode() == DiffOpenDialog::Dirs);
}

void DiffWindow::slotTreeViewFileSelected(const QString &file)
{
    Q_D(DiffWindow);
    d->compareFile(file);
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

#include "moc_diffwindow.cpp"
