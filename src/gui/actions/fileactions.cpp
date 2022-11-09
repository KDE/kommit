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

#include <KIO/ApplicationLauncherJob>
#include <KIO/OpenUrlJob>
#include <KLocalizedString>
#include <KMimeTypeTrader>
#include <kjobtrackerinterface.h>

#include <kio_version.h>
#if KIO_VERSION >= QT_VERSION_CHECK(5, 98, 0)
#include <KIO/JobUiDelegateFactory>
#else
#include <KIO/JobUiDelegate>
#endif

#include <QAction>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenu>
#include <QMimeDatabase>
#include <QStandardPaths>

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
    setActionEnabled(_actionOpen, !mPlace.isEmpty());
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

    _actionView = addAction(i18n("Preview"), this, &FileActions::viewFile, false, true);
    _actionOpen = addAction(i18n("Open"), this, &FileActions::openFile, false, true);
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
    auto d = new FileViewerDialog(mGit, mPlace, mFilePath, mParent);
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->show();
}

void FileActions::saveAsFile()

{
    const auto fileName = QFileDialog::getSaveFileName(mParent);
    if (!fileName.isEmpty()) {
        Git::File file{mGit, mPlace, mFilePath};
        file.save(fileName);
    }
}

void FileActions::logFile()
{
    const Git::File file{mGit, mPlace, mFilePath};
    FileHistoryDialog d(mGit, file, mParent);
    d.exec();
}

void FileActions::blameFile()
{
    const Git::File file(mGit, mPlace, mFilePath);
    FileBlameDialog d(mGit, file, mParent);
    d.exec();
}

void FileActions::search()
{
    SearchDialog d(mFilePath, mGit, mParent);
    d.exec();
}

void FileActions::openFile()
{
    const Git::File file(mGit, mPlace, mFilePath);
    auto tempFilePath = file.saveAsTemp(); // TODO: remove temp file after openning
    if (tempFilePath.isEmpty())
        return;

    auto url = QUrl::fromLocalFile(tempFilePath);
    qDebug() << "Openning " << tempFilePath << url;
    QDesktopServices::openUrl(url);

    // TODO: needs to be checked
    /*KIO::OpenUrlJob *job = new KIO::OpenUrlJob(url);

#if KIO_VERSION >= QT_VERSION_CHECK(5, 98, 0)
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
#else
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
#endif
    qDebug() << "Starting";
    if(!job->exec()) {
        qDebug() << job->errorString();
        if(!QDesktopServices::openUrl(url))
            qDebug() << "Could not launch external editor.";
    }*/
}

void FileActions::openWith()
{
    const Git::File file(mGit, mPlace, mFilePath);
    auto tempFilePath = file.saveAsTemp(); // TODO: remove temp file after openning
    if (tempFilePath.isEmpty())
        return;

    KIO::ApplicationLauncherJob *job = new KIO::ApplicationLauncherJob();
    job->setUrls({QUrl::fromLocalFile(tempFilePath)});

#if KIO_VERSION >= QT_VERSION_CHECK(5, 98, 0)
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
#else
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
#endif

    job->start();
}

void FileActions::diffWithHead()
{
    const Git::File oldFile{mGit, mPlace, mFilePath};
    const Git::File newFile{mGit->path() + QLatin1Char('/') + mFilePath};

    auto d = new DiffWindow(oldFile, newFile);
    d->showModal();
}

void FileActions::mergeWithHead()
{
    auto d = new MergeWindow(mGit, MergeWindow::NoParams);

    Git::File f{mGit, mPlace, mFilePath};
    auto tempFile = f.saveAsTemp();

    d->setFilePathBase(tempFile);
    d->setFilePathLocal(mGit->path() + QLatin1Char('/') + mFilePath);
    d->setFilePathRemote(tempFile);
    d->setFilePathResult(mGit->path() + QLatin1Char('/') + mFilePath);
    d->load();

    d->exec();
    QFile::remove(tempFile);
}
