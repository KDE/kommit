/*
Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergewindow.h"

#include "core/editactionsmapper.h"
#include "core/kmessageboxhelper.h"
#include "dialogs/mergecloseeventdialog.h"
#include "dialogs/mergeopenfilesdialog.h"
#include "libkommitwidgets_appdebug.h"
#include "widgets/codeeditor.h"
#include "widgets/mergewidget.h"
#include "widgets/segmentsmapper.h"

#include <KActionCollection>
#include <KLocalizedString>

#include "libkommitwidgets_appdebug.h"

#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMenu>
#include <QSettings>
#include <QStatusBar>
#include <QTextBlock>
#include <QTextEdit>
#include <entities/blob.h>

namespace MergeImpl
{

class Storage
{
public:
    enum class Mode { Null, File, Blob };

    Mode _mode{Mode::Null};
    QString _filePath;
    QString _title;

    QSharedPointer<Git::Blob> _blob;

    void setFile(const QString &path)
    {
        _mode = Mode::File;
        _filePath = path;
        _blob.reset();
    }
    void setFile(QSharedPointer<Git::Blob> file)
    {
        _mode = Mode::Blob;
        _blob = file;
    }

    QString title() const
    {
        switch (_mode) {
        case Mode::Null:
            return "{NULL}";

        case Mode::File:
            return _filePath;

        case Mode::Blob:
            return _blob->name();
        }
        return {};
    }

    QByteArray content() const
    {
        switch (_mode) {
        case Mode::Null:
            return "{NULL}";

        case Mode::Blob:
            return _blob->content();

        case Mode::File: {
            QFile f{_filePath};
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
                return {};
            auto buffer = f.readAll();
            f.close();
            return buffer;
        }
        }

        return {};
    }
};

}

class MergeWindowPrivate
{
    MergeWindow *q_ptr;
    Q_DECLARE_PUBLIC(MergeWindow)

public:
    explicit MergeWindowPrivate(MergeWindow *parent);

    QString resultFilePath;

    MergeWidget *mergeWidget{};
    MergeImpl::Storage base;
    MergeImpl::Storage local;
    MergeImpl::Storage remote;

    QLabel *conflictsLabel = nullptr;
    QAction *actionViewSameSizeBlocks = nullptr;

    // QAction *actionKeepMine = nullptr;
    // QAction *actionKeepTheir = nullptr;
    // QAction *actionKeepMineBeforeTheir = nullptr;
    // QAction *actionKeepTheirBeforeMine = nullptr;
    // QAction *actionKeepMyFile = nullptr;
    // QAction *actionKeepTheirFile = nullptr;
    // QAction *actionGotoPrevDiff = nullptr;
    // QAction *actionGotoNextDiff = nullptr;

    void initActions();
    bool saveResult(const QString &filePath);
};
MergeWindowPrivate::MergeWindowPrivate(MergeWindow *parent)
    : q_ptr{parent}
{
}

void MergeWindowPrivate::initActions()
{
    Q_Q(MergeWindow);

    KActionCollection *actionCollection = q->actionCollection();

    auto actionKeepMine = actionCollection->addAction(QStringLiteral("keep_mine"), mergeWidget->keepMineAction());
    actionKeepMine->setText(i18n("Keep mine"));
    actionKeepMine->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-mine")));
    actionCollection->setDefaultShortcut(actionKeepMine, QKeySequence(Qt::CTRL | Qt::Key_L));

    auto actionKeepTheir = actionCollection->addAction(QStringLiteral("keep_their"), mergeWidget->keepTheirAction());
    actionKeepTheir->setText(i18n("Keep their"));
    actionKeepTheir->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-their")));
    actionCollection->setDefaultShortcut(actionKeepTheir, QKeySequence(Qt::CTRL | Qt::Key_R));

    auto actionKeepMineBeforeTheir = actionCollection->addAction(QStringLiteral("keep_mine_before_their"), mergeWidget->keepMineBeforeTheirAction());
    actionKeepMineBeforeTheir->setText(i18n("Keep mine before their"));
    actionKeepMineBeforeTheir->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-mine-before-their")));
    actionCollection->setDefaultShortcut(actionKeepMineBeforeTheir, QKeySequence(QKeyCombination{Qt::CTRL | Qt::SHIFT, Qt::Key_L}));

    auto actionKeepTheirBeforeMine = actionCollection->addAction(QStringLiteral("keep_their_before_mine"), mergeWidget->keepTheirBeforeMineAction());
    actionKeepTheirBeforeMine->setText(i18n("Keep their before mine"));
    actionKeepTheirBeforeMine->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-their-before-mine")));
    actionCollection->setDefaultShortcut(actionKeepTheirBeforeMine, QKeySequence(QKeyCombination{Qt::CTRL | Qt::SHIFT, Qt::Key_R}));

    auto actionKeepMyFile = actionCollection->addAction(QStringLiteral("keep_my_file"), mergeWidget->keepMyFileAction());
    actionKeepMyFile->setText(i18n("Keep my file"));
    actionKeepMyFile->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-mine-file")));
    actionCollection->setDefaultShortcut(actionKeepMyFile, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_L));

    auto actionKeepTheirFile = actionCollection->addAction(QStringLiteral("keep_their_file"), mergeWidget->keepTheirFileAction());
    actionKeepTheirFile->setText(i18n("Keep their file"));
    actionKeepTheirFile->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-their-file")));
    actionCollection->setDefaultShortcut(actionKeepTheirFile, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_R));

    auto actionGotoPrevDiff = actionCollection->addAction(QStringLiteral("goto_prev_diff"), mergeWidget->gotoPrevDiffAction());
    actionGotoPrevDiff->setText(i18n("Previous diff"));
    actionGotoPrevDiff->setIcon(QIcon::fromTheme(QStringLiteral("diff-goto-prev-diff")));
    actionCollection->setDefaultShortcut(actionGotoPrevDiff, QKeySequence(Qt::Key_PageUp));

    auto actionGotoNextDiff = actionCollection->addAction(QStringLiteral("goto_next_diff"), mergeWidget->gotoNextDiffAction());
    actionGotoNextDiff->setText(i18n("Next diff"));
    actionGotoNextDiff->setIcon(QIcon::fromTheme(QStringLiteral("diff-goto-next-diff")));
    actionCollection->setDefaultShortcut(actionGotoNextDiff, QKeySequence(Qt::Key_PageDown));

    auto actionViewSameSizeBlocks = actionCollection->addAction(QStringLiteral("view_same_size_blocks"), q, &MergeWindow::slotSameSizeActivated);
    actionViewSameSizeBlocks->setText(i18n("Same size blocks"));
    actionViewSameSizeBlocks->setCheckable(true);
    actionViewSameSizeBlocks->setChecked(true);

    KStandardAction::open(q, &MergeWindow::fileOpen, actionCollection);
    KStandardAction::save(q, &MergeWindow::fileSave, actionCollection);
    KStandardAction::saveAs(q, &MergeWindow::fileSaveAs, actionCollection);
    KStandardAction::quit(q, &MergeWindow::close, actionCollection);

#ifdef UNDEF
    auto settingsManager = new SettingsManager(mGit, this);
    KStandardAction::preferences(settingsManager, &SettingsManager::show, actionCollection);
#endif

    // mCodeEditorContextMenu = new QMenu(this);
    // mCodeEditorContextMenu->addActions({d->actionKeepMine, d->actionKeepTheir});
    // mCodeEditorContextMenu->addSeparator();
    // mCodeEditorContextMenu->addActions({d->actionKeepMineBeforeTheir, d->actionKeepTheirBeforeMine});
    // mCodeEditorContextMenu->addSeparator();
    // mCodeEditorContextMenu->addActions({d->actionKeepMyFile, d->actionKeepTheirFile});
}

bool MergeWindowPrivate::saveResult(const QString &filePath)
{
    Q_Q(MergeWindow);
    QFile f(filePath);
    if (!f.open(QIODevice::Text | QIODevice::WriteOnly)) {
        return false;
    }
    f.write(mergeWidget->result().toUtf8());
    f.close();

    mergeWidget->setIsModified(false);
    return true;
}

MergeWindow::MergeWindow(Git::Repository *git, Mode mode, QWidget *parent)
    : AppMainWindow(parent)
    , d_ptr{new MergeWindowPrivate{this}}
{
    Q_UNUSED(mode)
    Q_UNUSED(git)
    Q_D(MergeWindow);

    // auto w = new QWidget(this);
    // m_ui.setupUi(w);
    // setCentralWidget(w);

    d->mergeWidget = new MergeWidget{this};
    setCentralWidget(d->mergeWidget);
    d->initActions();

    d->conflictsLabel = new QLabel(this);
    statusBar()->addPermanentWidget(d->conflictsLabel);

    setupGUI(Default, QStringLiteral("kommitmergeui.rc"));

    // QSettings s;
    // s.beginGroup(QStringLiteral("MergeWindow"));
    // if (s.value(QStringLiteral("actionType"), QStringLiteral("file")).toString() == QStringLiteral("file"))
    //     actionViewFilesClicked();
    // else
    //     actionViewBlocksClicked();

    connect(d->mergeWidget, &MergeWidget::conflictsChanged, this, &MergeWindow::slotMergeWidgetConflictsChanged);
    connect(d->mergeWidget, &MergeWidget::isModifiedChanged, this, &MergeWindow::setWindowModified);
}

MergeWindow::~MergeWindow()
{
    Q_D(MergeWindow);

    // QSettings s;
    // s.beginGroup(QStringLiteral("MergeWindow"));
    // s.setValue(QStringLiteral("actionType"), d->mActionFilesView->isChecked() ? QStringLiteral("file") : QStringLiteral("block"));
    // s.sync();
}

void MergeWindow::compare()
{
    Q_D(MergeWindow);
    d->mergeWidget->setBaseFile(d->base.title(), d->base.content());
    d->mergeWidget->setLocalFile(d->local.title(), d->local.content());
    d->mergeWidget->setRemoteFile(d->remote.title(), d->remote.content());

    if (d->resultFilePath.isEmpty()) {
        QFileInfo fiBase{d->base.title()};
        QFileInfo fiLocal{d->local.title()};
        QFileInfo fiRemote{d->remote.title()};

        QSet<QString> set;
        set << fiBase.completeSuffix();
        set << fiLocal.completeSuffix();
        set << fiRemote.completeSuffix();

        if (set.size() == 1)
            d->mergeWidget->setResultFile(QStringLiteral("sample-file.") + *set.begin());
    } else {
        d->mergeWidget->setResultFile(d->resultFilePath);
    }
    d->mergeWidget->compare();
}

void MergeWindow::closeEvent(QCloseEvent *event)
{
    Q_D(MergeWindow);
    if (d->mergeWidget->isModified()) {
        MergeCloseEventDialog d(this);
        auto r = d.exec();

        switch (r) {
        case MergeCloseEventDialog::MarkAsResolved:
            fileSave();
            accept();
            break;
        case MergeCloseEventDialog::LeaveAsIs:
            reject();
            break;
        case MergeCloseEventDialog::DontExit:
            event->ignore();
            break;
        }
    }
    accept();
}

void MergeWindow::setResultFile(const QString &newFilePathResult)
{
    Q_D(MergeWindow);
    d->resultFilePath = newFilePathResult;
    setWindowFilePath(newFilePathResult);
}

void MergeWindow::fileSave()
{
    Q_D(MergeWindow);
    if (d->resultFilePath.isEmpty()) {
        fileSaveAs();
        return;
    }
    if (!d->saveResult(d->resultFilePath))
        KMessageBoxHelper::information(this, i18n("Unable to open the file %1", d->resultFilePath));
}

void MergeWindow::fileSaveAs()
{
    Q_D(MergeWindow);

    d->resultFilePath = QFileDialog::getSaveFileName(this, i18n("Save result"));

    if (d->resultFilePath.isEmpty())
        return;

    setWindowFilePath(d->resultFilePath);
    if (!d->saveResult(d->resultFilePath))
        KMessageBoxHelper::information(this, i18n("Unable to open the file %1", d->resultFilePath));
}

void MergeWindow::fileOpen()
{
    MergeOpenFilesDialog d;
    if (d.exec() == QDialog::Accepted) {
        setBaseFile(d.filePathBase());
        setLocalFile(d.filePathLocal());
        setRemoteFile(d.filePathRemote());
        compare();
    }
}

void MergeWindow::slotSameSizeActivated()
{
    Q_D(MergeWindow);
    d->mergeWidget->setSameSize(d->actionViewSameSizeBlocks->isChecked());
}

void MergeWindow::setBaseFile(const QString &filePath)
{
    Q_D(MergeWindow);
    d->base.setFile(filePath);
}

void MergeWindow::setBaseFile(QSharedPointer<Git::Blob> blob)
{
    Q_D(MergeWindow);
    d->base.setFile(blob);
}

void MergeWindow::setRemoteFile(const QString &filePath)
{
    Q_D(MergeWindow);
    d->remote.setFile(filePath);
}

void MergeWindow::setRemoteFile(QSharedPointer<Git::Blob> blob)
{
    Q_D(MergeWindow);
    d->remote.setFile(blob);
}

void MergeWindow::setLocalFile(const QString &filePath)
{
    Q_D(MergeWindow);
    d->local.setFile(filePath);
}

void MergeWindow::setLocalFile(QSharedPointer<Git::Blob> blob)
{
    Q_D(MergeWindow);
    d->local.setFile(blob);
}

void MergeWindow::slotMergeWidgetConflictsChanged(int conflicts)
{
    Q_D(MergeWindow);

    if (conflicts)
        d->conflictsLabel->setText(i18n("Conflicts: <span style='font-weight:bold; color:red;'>%1</span>", conflicts));
    else
        d->conflictsLabel->setText(i18n("<span style='color:green;'>All conflicts resolved</span>"));
}

#include "moc_mergewindow.cpp"
