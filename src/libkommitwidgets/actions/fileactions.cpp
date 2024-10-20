/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileactions.h"

#include "dialogs/filehistorydialog.h"
#include "dialogs/fileviewerdialog.h"
#include "dialogs/searchdialog.h"
#include "windows/diffwindow.h"
#include "windows/mergewindow.h"

#include <entities/file.h>
#include <repository.h>

#include <KIO/ApplicationLauncherJob>
#include <KIO/OpenUrlJob>
#include <KLocalizedString>
#include <kjobtrackerinterface.h>

#include <kio_version.h>
#if KIO_VERSION >= QT_VERSION_CHECK(5, 98, 0)
#include <KIO/JobUiDelegateFactory>
#else
#include <KIO/JobUiDelegate>
#endif

#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenu>
#include <QMimeDatabase>
#include <QStandardPaths>

FileActions::FileActions(Git::Repository *git, QWidget *parent)
    : AbstractActions(git, parent)
    , mOpenWithMenu(new QMenu(parent))
{
    _actionView = addAction(i18nc("@action", "Preview"), this, &FileActions::viewFile, false, true);
    _actionOpen = addAction(i18nc("@action", "Open"), this, &FileActions::openFile, false, true);
    _actionOpenWith = addAction(i18nc("@action", "Open with…"), this, &FileActions::openWith, false, true);

    _actionDiffWithHead = addAction(i18nc("@action", "Diff with working dir"), this, &FileActions::diffWithHead, false, true);
    _actionMergeWithHead = addAction(i18nc("@action", "Merge with working dir"), this, &FileActions::mergeWithHead, false, true);

    _actionSaveAs = addAction(i18nc("@action", "Save as…"), this, &FileActions::saveAsFile, false, true);
    _actionHistory = addAction(i18nc("@action", "Log"), this, &FileActions::logFile, false, true);
    _actionSearch = addAction(i18nc("@action", "Search…"), this, &FileActions::search, false, true);
}

void FileActions::popup(const QPoint &pos)
{
    mMenu->popup(pos);
}

void FileActions::viewFile()
{
    auto d = new FileViewerDialog(mFile, mParent);
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->show();
}

void FileActions::saveAsFile()
{
    QFileInfo fi{mFile.filePath()};
    const auto filter = i18n("%1 file (*.%1);;All files(*)", fi.suffix());
    const auto fileName = QFileDialog::getSaveFileName(mParent, {}, mFile.filePath(), filter);

    if (!fileName.isEmpty()) {
        mFile.save(fileName);
    }
}

void FileActions::logFile()
{
    FileHistoryDialog d(mGit, mFile, mParent);
    d.exec();
}

void FileActions::search()
{
    qWarning() << "FileActions::search() not implemented yet ";
    // SearchDialog d(mFilePath, mGit, mParent);
    // d.exec();
}

void FileActions::openFile()
{
    auto tempFilePath = mFile.saveAsTemp(); // TODO: remove temp file after openning
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
    auto tempFilePath = mFile.saveAsTemp(); // TODO: remove temp file after openning
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
    Git::Blob newFile{mGit->repoPtr(), mFile.filePath()};

    auto d = new DiffWindow(mFile, newFile);
    d->showModal();
}

void FileActions::mergeWithHead()
{
    auto d = new MergeWindow(mGit, MergeWindow::NoParams);

    auto tempFile = mFile.saveAsTemp();

    d->setBaseFile(tempFile);
    d->setLocalFile(mGit->path() + QLatin1Char('/') + mFile.filePath());
    d->setRemoteFile(tempFile);
    d->setResultFile(mGit->path() + QLatin1Char('/') + mFile.filePath());
    d->compare();

    d->exec();
    QFile::remove(tempFile);
}

const Git::Blob &FileActions::file() const
{
    return mFile;
}

void FileActions::setFile(const Git::Blob &file)
{
    mFile = file;

    setActionEnabled(_actionView, !mFile.isNull());
    setActionEnabled(_actionOpen, !mFile.isNull());
    setActionEnabled(_actionOpenWith, !mFile.isNull());
    setActionEnabled(_actionDiffWithHead, !mFile.isNull());
    setActionEnabled(_actionMergeWithHead, !mFile.isNull());
    setActionEnabled(_actionSaveAs, !mFile.isNull());
    setActionEnabled(_actionHistory, !mFile.isNull());
    setActionEnabled(_actionSearch, !mFile.isNull());
}

#include "moc_fileactions.cpp"
