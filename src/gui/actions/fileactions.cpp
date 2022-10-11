/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileactions.h"

#include "dialogs/fileblamedialog.h"
#include "dialogs/filehistorydialog.h"
#include "dialogs/fileviewerdialog.h"
#include "dialogs/searchdialog.h"
#include "diffwindow.h"
#include "gitfile.h"
#include "gitmanager.h"
#include "mergewindow.h"

#include <KLocalizedString>
#include <KMimeTypeTrader>
#include <KRun>

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMimeDatabase>

KService::Ptr FileActions::getExternalViewer(const QString &mimeType)
{
    KService::List offers = KMimeTypeTrader::self()->query(mimeType, QStringLiteral("Application"));

    if (!offers.isEmpty())
        return offers.first();
    return {};
}

const QString &FileActions::place() const
{
    return mPlace;
}

void FileActions::setPlace(const QString &newPlace)
{
    mPlace = newPlace;

    setActionEnabled(_actionView, !mFilePath.isEmpty());
    setActionEnabled(_actionOpenWith, !mFilePath.isEmpty());
    setActionEnabled(_actionDiffWithHead, !mFilePath.isEmpty());
    setActionEnabled(_actionMergeWithHead, !mFilePath.isEmpty());
    setActionEnabled(_actionSaveAs, !mFilePath.isEmpty());
    setActionEnabled(_actionHistory, !mFilePath.isEmpty());
    setActionEnabled(_actionBlame, !mFilePath.isEmpty());
    setActionEnabled(_actionSearch, !mFilePath.isEmpty());
}

const QString &FileActions::filePath() const
{
    return mFilePath;
}

void FileActions::setFilePath(const QString &newFilePath)
{
    mFilePath = newFilePath;

    setActionEnabled(_actionView, !mPlace.isEmpty());
    setActionEnabled(_actionOpenWith, !mPlace.isEmpty());
    setActionEnabled(_actionDiffWithHead, !mPlace.isEmpty());
    setActionEnabled(_actionMergeWithHead, !mPlace.isEmpty());
    setActionEnabled(_actionSaveAs, !mPlace.isEmpty());
    setActionEnabled(_actionHistory, !mPlace.isEmpty());
    setActionEnabled(_actionBlame, !mPlace.isEmpty());
    setActionEnabled(_actionSearch, !mPlace.isEmpty());
}

FileActions::FileActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    mOpenWithMenu = new QMenu(parent);

    _actionView = addAction(i18n("View..."), this, &FileActions::viewFile, false, true);
    _actionOpenWith = addAction(i18n("Open with..."), this, &FileActions::openWith, false, true);

    _actionDiffWithHead = addAction(i18n("Diff with HEAD..."), this, &FileActions::diffWithHead, false, true);
    _actionMergeWithHead = addAction(i18n("Merge with HEAD..."), this, &FileActions::mergeWithHead, false, true);

    _actionSaveAs = addAction(i18n("Save as..."), this, &FileActions::saveAsFile, false, true);
    _actionHistory = addAction(i18n("Log..."), this, &FileActions::logFile, false, true);
    _actionBlame = addAction(i18n("Blame..."), this, &FileActions::blameFile, false, true);
    _actionSearch = addAction(i18n("Search..."), this, &FileActions::search, false, true);
}

void FileActions::popup(const QPoint &pos)
{
    mMenu->popup(pos);
}

void FileActions::viewFile()
{
    auto d = new FileViewerDialog(mPlace, mFilePath, mParent);
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->show();
}

void FileActions::saveAsFile()
{
    const auto fileName = QFileDialog::getSaveFileName(mParent);
    if (!fileName.isEmpty()) {
        Git::File file{mPlace, mFilePath};
        file.save(fileName);
    }
}

void FileActions::logFile()
{
    const Git::File file{mPlace, mFilePath};
    FileHistoryDialog d(mGit, file, mParent);
    d.exec();
}

void FileActions::blameFile()
{
    //    auto path = _treeModel->fullPath(treeView->currentIndex()) + "/"
    //                + listWidget->currentItem()->text();
    const Git::File file(mPlace, mFilePath, mGit);
    FileBlameDialog d(file, mParent);
    d.exec();
}

void FileActions::search()
{
    //    auto path = _treeModel->fullPath(treeView->currentIndex()) + "/"
    //                + listWidget->currentItem()->text();
    SearchDialog d(mFilePath, Git::Manager::instance(), mParent);
    d.exec();
}

KService::Ptr FileActions::getViewer(const QString &mimeType)
{
    // No point in even trying to find anything for application/octet-stream
    if (mimeType == QStringLiteral("application/octet-stream")) {
        return {};
    }

    // Try to get a read-only kpart for the internal viewer
    KService::List offers = KMimeTypeTrader::self()->query(mimeType, QStringLiteral("KParts/ReadOnlyPart"));

    // If we can't find a kpart, try to get an external application
    if (offers.isEmpty()) {
        offers = KMimeTypeTrader::self()->query(mimeType, QStringLiteral("Application"));
    }

    if (!offers.isEmpty())
        return offers.first();
    return {};
}

void FileActions::openWith()
{
    auto fileName = mGit->path() + QLatin1Char('/') + mFilePath;
    const QList<QUrl> fileUrlList = {QUrl::fromLocalFile(fileName)};
    QMimeDatabase db;
    const QMimeType mimeType = db.mimeTypeForFile(fileName);
    auto viewer = getViewer(mimeType.name());
    KRun::runService(*viewer, fileUrlList, nullptr, true);

    //    KIO::ApplicationLauncherJob *job = new KIO::ApplicationLauncherJob(viewer);
    //    job->setUrls(fileUrlList);
    //    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
    //    // The temporary file will be removed when the viewer application exits.
    //    job->setRunFlags(KIO::ApplicationLauncherJob::DeleteTemporaryFiles);
    //    job->start();
}

void FileActions::diffWithHead()
{
    const Git::File oldFile{mPlace, mFilePath};
    const Git::File newFile{mGit->path() + QLatin1Char('/') + mFilePath};

    auto d = new DiffWindow(oldFile, newFile);
    d->showModal();
}

void FileActions::mergeWithHead()
{
    auto d = new MergeWindow(MergeWindow::NoParams);

    auto p = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/ggggg");
    Git::File f{mPlace, mFilePath};
    f.save(p);

    d->setFilePathBase(p);
    d->setFilePathLocal(mGit->path() + QLatin1Char('/') + mFilePath);
    d->setFilePathRemote(p);
    d->setFilePathResult(mGit->path() + QLatin1Char('/') + mFilePath);
    d->load();

    d->showModal();
}
