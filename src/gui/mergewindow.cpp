/*
Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergewindow.h"

#include "dialogs/mergecloseeventdialog.h"
#include "dialogs/mergeopenfilesdialog.h"
#include "diff/segmentsmapper.h"
#include "settings/settingsmanager.h"
#include "widgets/codeeditor.h"
#include "widgets/editactionsmapper.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>

#include "gitklient_appdebug.h"
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMenu>
#include <QSettings>
#include <QStatusBar>
#include <QTextBlock>
#include <QTextEdit>

bool isEmpty(const QStringList &list)
{
    if (list.isEmpty())
        return true;

    for (const auto &s : list)
        if (!s.trimmed().isEmpty())
            return false;
    return true;
}
void compare(QTextEdit *e1, QTextEdit *e2)
{
    auto m = qMin(e1->document()->blockCount(), e2->document()->blockCount());
    for (int i = 0; i < m; ++i) {
        auto block1 = e1->document()->findBlock(i);
        auto block2 = e2->document()->findBlock(i);
        if (block1.text() != block2.text()) {
            //            block1.blockFormat()
        }
    }
}
QString readFile(const QString &filePath)
{
    QFile f{filePath};
    if (!f.open(QIODevice::ReadOnly))
        return {};

    const auto buf = QString(f.readAll());
    f.close();
    return buf;
}

MergeWindow::MergeWindow(Git::Manager *git, Mode mode, QWidget *parent)
    : AppMainWindow(parent)
    , mGit(git)
{
    Q_UNUSED(mode)
    auto w = new QWidget(this);
    m_ui.setupUi(w);
    setCentralWidget(w);

    initActions();
    init();

    QSettings s;
    s.beginGroup(QStringLiteral("MergeWindow"));
    if (s.value(QStringLiteral("actionType"), QStringLiteral("file")).toString() == QStringLiteral("file"))
        actionViewFiles_clicked();
    else
        actionViewBlocks_clicked();
}

MergeWindow::~MergeWindow()
{
    QSettings s;
    s.beginGroup(QStringLiteral("MergeWindow"));
    s.setValue(QStringLiteral("actionType"), mActionFilesView->isChecked() ? QStringLiteral("file") : QStringLiteral("block"));
    s.sync();
}

void MergeWindow::init()
{
    auto mapper = new EditActionsMapper;
    mapper->init(actionCollection());

    mapper->addTextEdit(m_ui.plainTextEditMine);
    mapper->addTextEdit(m_ui.plainTextEditTheir);
    mapper->addTextEdit(m_ui.plainTextEditResult);

    mMapper = new SegmentsMapper;

    mMapper->addEditor(m_ui.plainTextEditBase);
    mMapper->addEditor(m_ui.plainTextEditMine);
    mMapper->addEditor(m_ui.plainTextEditTheir);
    mMapper->addEditor(m_ui.plainTextEditResult);

    m_ui.plainTextEditMine->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ui.plainTextEditTheir->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_ui.plainTextEditMine, &CodeEditor::customContextMenuRequested, this, &MergeWindow::codeEditors_customContextMenuRequested);
    connect(m_ui.plainTextEditTheir, &CodeEditor::customContextMenuRequested, this, &MergeWindow::codeEditors_customContextMenuRequested);
    connect(m_ui.plainTextEditResult, &CodeEditor::blockSelected, this, &MergeWindow::slotPlainTextEditResultBlockSelected);
    connect(m_ui.plainTextEditResult, &CodeEditor::blockSelected, this, &MergeWindow::slotPlainTextEditResultBlockSelected);
    connect(m_ui.plainTextEditResult, &CodeEditor::blockSelected, this, &MergeWindow::slotPlainTextEditResultBlockSelected);
    connect(m_ui.plainTextEditResult, &CodeEditor::blockSelected, this, &MergeWindow::slotPlainTextEditResultBlockSelected);
    connect(m_ui.plainTextEditResult, &CodeEditor::textChanged, this, &MergeWindow::slotPlainTextEditResultTextChanged);
    connect(m_ui.plainTextEditResult, &CodeEditor::blockSelected, this, &MergeWindow::slotPlainTextEditResultBlockSelected);

    mConflictsLabel = new QLabel(this);
    statusBar()->addPermanentWidget(mConflictsLabel);

    actionViewBlocks_clicked();

    setupGUI(Default, QStringLiteral("gitklientmergeui.rc"));
}

void MergeWindow::fillSegments()
{
    m_ui.plainTextEditMine->clearAll();
    m_ui.plainTextEditBase->clearAll();
    m_ui.plainTextEditTheir->clearAll();
    for (const auto &d : std::as_const(mDiffs)) {
        auto blockSize = actionViewSameSizeBlocks->isChecked() ? qMax(d->base.size(), qMax(d->remote.size(), d->local.size())) : -1;

        switch (d->type) {
        case Diff::SegmentType::SameOnBoth: {
            m_ui.plainTextEditMine->append(d->base, CodeEditor::Unchanged, d, blockSize);
            m_ui.plainTextEditTheir->append(d->base, CodeEditor::Unchanged, d, blockSize);
            m_ui.plainTextEditBase->append(d->base, CodeEditor::Unchanged, d, blockSize);
            d->mergeType = Diff::KeepLocal;
            break;
        }

        case Diff::SegmentType::OnlyOnRight:
            m_ui.plainTextEditMine->append(d->local, CodeEditor::Removed, d, blockSize);
            m_ui.plainTextEditTheir->append(d->remote, CodeEditor::Added, d, blockSize);
            m_ui.plainTextEditBase->append(d->base, CodeEditor::Edited, d, blockSize);
            d->mergeType = Diff::KeepRemote;
            break;
        case Diff::SegmentType::OnlyOnLeft:
            m_ui.plainTextEditMine->append(d->local, CodeEditor::Added, d, blockSize);
            m_ui.plainTextEditTheir->append(d->remote, CodeEditor::Removed, d, blockSize);
            m_ui.plainTextEditBase->append(d->base, CodeEditor::Edited, d, blockSize);
            d->mergeType = Diff::KeepLocal;
            break;

        case Diff::SegmentType::DifferentOnBoth:
            if (isEmpty(d->local)) {
                m_ui.plainTextEditMine->append(d->local, CodeEditor::Edited, d, blockSize);
                m_ui.plainTextEditTheir->append(d->remote, CodeEditor::Added, d, blockSize);
                //                d->mergeType = Diff::KeepRemote;
            } else if (isEmpty(d->remote)) {
                m_ui.plainTextEditMine->append(d->local, CodeEditor::Added, d, blockSize);
                m_ui.plainTextEditTheir->append(d->remote, CodeEditor::Edited, d, blockSize);
                //                d->mergeType = Diff::KeepLocal;
            } else {
                m_ui.plainTextEditMine->append(d->local, CodeEditor::Edited, d, blockSize);
                m_ui.plainTextEditTheir->append(d->remote, CodeEditor::Edited, d, blockSize);
            }
            m_ui.plainTextEditBase->append(d->base, CodeEditor::Edited, d, blockSize);
            d->mergeType = Diff::None;
            break;
        }
    }
}

void MergeWindow::load()
{
    m_ui.plainTextEditMine->clear();
    m_ui.plainTextEditTheir->clear();
    m_ui.plainTextEditResult->clear();
    m_ui.plainTextEditBase->clear();

    auto baseList = readFile(mFilePathBase);
    auto localList = readFile(mFilePathLocal);
    auto remoteList = readFile(mFilePathRemote);

    m_ui.plainTextEditBase->setHighlighting(mFilePathBase);
    m_ui.plainTextEditMine->setHighlighting(mFilePathLocal);
    m_ui.plainTextEditTheir->setHighlighting(mFilePathRemote);
    m_ui.plainTextEditResult->setHighlighting(mFilePathResult);
    m_ui.codeEditorMyBlock->setHighlighting(mFilePathLocal);
    m_ui.codeEditorTheirBlock->setHighlighting(mFilePathRemote);

    auto result = Diff::diff3(baseList, localList, remoteList);
    mDiffs = result.segments;
    mMapper->setSegments(mDiffs);
    QList<Diff::Segment *> segments;
    for (const auto &s : std::as_const(mDiffs)) {
        segments.append(s);
    }

    fillSegments();
    updateResult();

    QFileInfo fi;
    if (mFilePathResult.isEmpty())
        fi.setFile(mFilePathBase);
    else
        fi.setFile(mFilePathResult);

    setWindowTitle(fi.fileName() + QStringLiteral("[*]"));
    setWindowModified(true);

    actionKeepMine->setEnabled(true);
    actionKeepTheir->setEnabled(true);
    actionKeepMineBeforeTheir->setEnabled(true);
    actionKeepTheirBeforeMine->setEnabled(true);
    actionKeepMyFile->setEnabled(true);
    actionKeepTheirFile->setEnabled(true);
    actionGotoPrevDiff->setEnabled(true);
    actionGotoNextDiff->setEnabled(true);
}

void MergeWindow::updateResult()
{
    m_ui.plainTextEditResult->clearAll();
    for (const auto &d : std::as_const(mDiffs)) {
        auto blockSize = actionViewSameSizeBlocks->isChecked() ? qMax(d->base.size(), qMax(d->remote.size(), d->local.size())) : -1;

        if (d->type == Diff::SegmentType::SameOnBoth) {
            m_ui.plainTextEditResult->append(d->base, CodeEditor::Unchanged, d, blockSize);
            continue;
        }
        switch (d->mergeType) {
        case Diff::None: {
            switch (d->type) {
            case Diff::SegmentType::SameOnBoth:
                m_ui.plainTextEditResult->append(d->base, CodeEditor::Unchanged, d, blockSize);
                break;

            case Diff::SegmentType::OnlyOnRight:
                m_ui.plainTextEditResult->append(d->remote, CodeEditor::Added, d, blockSize);
                break;

            case Diff::SegmentType::OnlyOnLeft:
                m_ui.plainTextEditResult->append(d->local, CodeEditor::Added, d, blockSize);
                break;

            case Diff::SegmentType::DifferentOnBoth:
                if (d->local == d->remote)
                    m_ui.plainTextEditResult->append(d->remote, CodeEditor::Added, d, blockSize); // Not changed
                else if (isEmpty(d->local))
                    m_ui.plainTextEditResult->append(d->remote, CodeEditor::Added, d, blockSize);
                else if (isEmpty(d->remote))
                    m_ui.plainTextEditResult->append(d->local, CodeEditor::Added, d, blockSize);
                else
                    m_ui.plainTextEditResult->append(QStringLiteral(" "), CodeEditor::Removed, d);
                break;
            }
            break;
        }

        case Diff::KeepLocal:
            m_ui.plainTextEditResult->append(d->local, CodeEditor::Edited, d, blockSize);
            break;

        case Diff::KeepRemote:
            m_ui.plainTextEditResult->append(d->remote, CodeEditor::Edited, d, blockSize);
            break;

        case Diff::KeepLocalThenRemote:
            m_ui.plainTextEditResult->append(d->local, CodeEditor::Edited, d, blockSize);
            m_ui.plainTextEditResult->append(d->remote, CodeEditor::Edited, d, blockSize);
            break;

        case Diff::KeepRemoteThenLocal:
            m_ui.plainTextEditResult->append(d->remote, CodeEditor::Edited, d, blockSize);
            m_ui.plainTextEditResult->append(d->local, CodeEditor::Edited, d, blockSize);
            break;

        default:
            m_ui.plainTextEditResult->append(QStringLiteral("***"), CodeEditor::Edited, d);
            break;
        }
    }
    mConflictsLabel->setText(i18n("Conflicts: %1", mMapper->conflicts()));
}

void MergeWindow::initActions()
{
    KActionCollection *actionCollection = this->actionCollection();

    actionKeepMine = actionCollection->addAction(QStringLiteral("keep_mine"), this, &MergeWindow::actionKeepMine_clicked);
    actionKeepMine->setText(i18n("Keep mine"));
    actionKeepMine->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-mine")));
    actionCollection->setDefaultShortcut(actionKeepMine, QKeySequence(Qt::CTRL | Qt::Key_L));

    actionKeepTheir = actionCollection->addAction(QStringLiteral("keep_their"), this, &MergeWindow::actionKeepTheir_clicked);
    actionKeepTheir->setText(i18n("Keep their"));
    actionKeepTheir->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-their")));
    actionCollection->setDefaultShortcut(actionKeepTheir, QKeySequence(Qt::CTRL | Qt::Key_R));

    actionKeepMineBeforeTheir = actionCollection->addAction(QStringLiteral("keep_mine_before_their"), this, &MergeWindow::actionKeepMineBeforeTheir_clicked);

    actionKeepMineBeforeTheir->setText(i18n("Keep mine before their"));
    actionKeepMineBeforeTheir->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-mine-before-their")));
    actionCollection->setDefaultShortcut(actionKeepMineBeforeTheir, QKeySequence(Qt::CTRL | Qt::Key_L | Qt::SHIFT));

    actionKeepTheirBeforeMine = actionCollection->addAction(QStringLiteral("keep_their_before_mine"), this, &MergeWindow::actionKeepTheirBeforeMine_clicked);
    actionKeepTheirBeforeMine->setText(i18n("Keep their before mine"));
    actionKeepTheirBeforeMine->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-their-before-mine")));
    actionCollection->setDefaultShortcut(actionKeepTheirBeforeMine, QKeySequence(Qt::CTRL | Qt::Key_R | Qt::SHIFT));

    actionKeepMyFile = actionCollection->addAction(QStringLiteral("keep_my_file"), this, &MergeWindow::actionKeepMyFile_clicked);
    actionKeepMyFile->setText(i18n("Keep my file"));
    actionKeepMyFile->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-mine-file")));
    actionCollection->setDefaultShortcut(actionKeepMyFile, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_L));

    actionKeepTheirFile = actionCollection->addAction(QStringLiteral("keep_their_file"), this, &MergeWindow::actionKeepTheirFile_clicked);
    actionKeepTheirFile->setText(i18n("Keep their file"));
    actionKeepTheirFile->setIcon(QIcon::fromTheme(QStringLiteral("diff-keep-their-file")));
    actionCollection->setDefaultShortcut(actionKeepTheirFile, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_R));

    mActionBlocksView = actionCollection->addAction(QStringLiteral("view_blocks"), this, &MergeWindow::actionViewBlocks_clicked);
    mActionBlocksView->setText(i18n("Blocks"));
    mActionBlocksView->setCheckable(true);

    mActionFilesView = actionCollection->addAction(QStringLiteral("view_files"), this, &MergeWindow::actionViewFiles_clicked);
    mActionFilesView->setText(i18n("Files"));
    mActionFilesView->setCheckable(true);

    actionGotoPrevDiff = actionCollection->addAction(QStringLiteral("goto_prev_diff"), this, &MergeWindow::actionGotoPrevDiff_clicked);
    actionGotoPrevDiff->setText(i18n("Previous diff"));
    actionGotoPrevDiff->setIcon(QIcon::fromTheme(QStringLiteral("diff-goto-prev-diff")));
    actionGotoPrevDiff->setEnabled(false);
    actionCollection->setDefaultShortcut(actionGotoPrevDiff, QKeySequence(Qt::Key_PageUp));

    actionGotoNextDiff = actionCollection->addAction(QStringLiteral("goto_next_diff"), this, &MergeWindow::actionGotoNextDiff_clicked);
    actionGotoNextDiff->setText(i18n("Next diff"));
    actionGotoNextDiff->setIcon(QIcon::fromTheme(QStringLiteral("diff-goto-next-diff")));
    actionCollection->setDefaultShortcut(actionGotoNextDiff, QKeySequence(Qt::Key_PageDown));

    actionViewSameSizeBlocks = actionCollection->addAction(QStringLiteral("view_same_size_blocks"), this, &MergeWindow::fillSegments);
    actionViewSameSizeBlocks->setText(i18n("Same size blocks"));
    actionViewSameSizeBlocks->setCheckable(true);
    actionViewSameSizeBlocks->setChecked(true);

    KStandardAction::open(this, &MergeWindow::fileOpen, actionCollection);
    KStandardAction::save(this, &MergeWindow::fileSave, actionCollection);
    KStandardAction::quit(qApp, &QApplication::closeAllWindows, actionCollection);

    auto settingsManager = new SettingsManager(mGit, this);
    KStandardAction::preferences(settingsManager, &SettingsManager::show, actionCollection);

    mCodeEditorContextMenu = new QMenu(this);
    mCodeEditorContextMenu->addActions({actionKeepMine, actionKeepTheir});
    mCodeEditorContextMenu->addSeparator();
    mCodeEditorContextMenu->addActions({actionKeepMineBeforeTheir, actionKeepTheirBeforeMine});
    mCodeEditorContextMenu->addSeparator();
    mCodeEditorContextMenu->addActions({actionKeepMyFile, actionKeepTheirFile});

    actionKeepMine->setEnabled(false);
    actionKeepTheir->setEnabled(false);
    actionKeepMineBeforeTheir->setEnabled(false);
    actionKeepTheirBeforeMine->setEnabled(false);
    actionKeepMyFile->setEnabled(false);
    actionKeepTheirFile->setEnabled(false);
    actionGotoPrevDiff->setEnabled(false);
    actionGotoNextDiff->setEnabled(false);
}

void MergeWindow::doMergeAction(Diff::MergeType type)
{
    auto s = mMapper->currentSegment();

    if (!s)
        return;

    if (s->type == Diff::SegmentType::SameOnBoth)
        return;

    auto ss = static_cast<Diff::MergeSegment *>(s);
    ss->mergeType = type;
    updateResult();
    //    m_ui.plainTextEditResult->highlightSegment(s);

    mMapper->setCurrentSegment(s);

    setWindowModified(true);
}

bool MergeWindow::isFullyResolved() const
{
    for (const auto &d : std::as_const(mDiffs))
        if (d->mergeType == Diff::None && d->type == Diff::SegmentType::DifferentOnBoth)
            return false;
    return true;
}

void MergeWindow::closeEvent(QCloseEvent *event)
{
    if (isWindowModified()) {
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

const QString &MergeWindow::filePathResult() const
{
    return mFilePathResult;
}

void MergeWindow::setFilePathResult(const QString &newFilePathResult)
{
    mFilePathResult = newFilePathResult;
}

void MergeWindow::fileSave()
{
    QFile f(mFilePathResult);
    if (!f.open(QIODevice::Text | QIODevice::WriteOnly)) {
        KMessageBox::information(this, i18n("Unable to open the file") + mFilePathResult);
        return;
    }
    f.write(m_ui.plainTextEditResult->toPlainText().toUtf8());
    f.close();
    setWindowModified(false);
}

void MergeWindow::fileOpen()
{
    MergeOpenFilesDialog d;
    if (d.exec() == QDialog::Accepted) {
        setFilePathBase(d.filePathBase());
        setFilePathLocal(d.filePathLocal());
        setFilePathRemote(d.filePathRemote());
        load();
    }
}

void MergeWindow::actionKeepMine_clicked()
{
    doMergeAction(Diff::MergeType::KeepLocal);
}

void MergeWindow::actionKeepTheir_clicked()
{
    doMergeAction(Diff::MergeType::KeepRemote);
}

void MergeWindow::actionKeepMineBeforeTheir_clicked()
{
    doMergeAction(Diff::MergeType::KeepLocalThenRemote);
}

void MergeWindow::actionKeepTheirBeforeMine_clicked()
{
    doMergeAction(Diff::MergeType::KeepRemoteThenLocal);
}

void MergeWindow::actionKeepMyFile_clicked()
{
    m_ui.plainTextEditResult->setPlainText(m_ui.plainTextEditMine->toPlainText());
}

void MergeWindow::actionKeepTheirFile_clicked()
{
    m_ui.plainTextEditResult->setPlainText(m_ui.plainTextEditTheir->toPlainText());
}

void MergeWindow::actionGotoPrevDiff_clicked()
{
    mMapper->findPrevious(Diff::SegmentType::DifferentOnBoth);
    slotPlainTextEditResultBlockSelected();
}

void MergeWindow::actionGotoNextDiff_clicked()
{
    mMapper->findNext(Diff::SegmentType::DifferentOnBoth);
    slotPlainTextEditResultBlockSelected();
}

void MergeWindow::actionViewFiles_clicked()
{
    mActionBlocksView->setChecked(false);
    mActionFilesView->setChecked(true);
    m_ui.widgetBlockView->hide();
    m_ui.widgetCodeView->show();
}

void MergeWindow::actionViewBlocks_clicked()
{
    mActionBlocksView->setChecked(true);
    mActionFilesView->setChecked(false);
    m_ui.widgetBlockView->show();
    m_ui.widgetCodeView->hide();
}

void MergeWindow::codeEditors_customContextMenuRequested(QPoint pos)
{
    Q_UNUSED(pos)
    mCodeEditorContextMenu->popup(QCursor::pos());
}

const QString &MergeWindow::filePathBase() const
{
    return mFilePathBase;
}

void MergeWindow::setFilePathBase(const QString &newFilePathBase)
{
    mFilePathBase = newFilePathBase;
}

const QString &MergeWindow::filePathRemote() const
{
    return mFilePathRemote;
}

void MergeWindow::setFilePathRemote(const QString &newFilePathRemote)
{
    mFilePathRemote = newFilePathRemote;
}

const QString &MergeWindow::filePathLocal() const
{
    return mFilePathLocal;
}

void MergeWindow::setFilePathLocal(const QString &newFilePathLocal)
{
    mFilePathLocal = newFilePathLocal;
}

void MergeWindow::slotPlainTextEditResultTextChanged()
{
    qCDebug(GITKLIENT_LOG) << "**********************";
    //    auto segment = static_cast<Diff::MergeSegment *>(_mapper->currentSegment());
    //    if (segment) {
    //        segment->mergeType = Diff::MergeCustom;
    //    }
}

void MergeWindow::slotPlainTextEditResultBlockSelected()
{
    auto segment = static_cast<Diff::MergeSegment *>(m_ui.plainTextEditResult->currentSegment());

    if (!segment)
        return;
    if (segment->type == Diff::SegmentType::DifferentOnBoth) {
        m_ui.codeEditorMyBlock->setPlainText(segment->local.join(QStringLiteral("\n")));
        m_ui.codeEditorTheirBlock->setPlainText(segment->remote.join(QStringLiteral("\n")));
    } else {
        m_ui.codeEditorMyBlock->clear();
        m_ui.codeEditorTheirBlock->clear();
    }
}
