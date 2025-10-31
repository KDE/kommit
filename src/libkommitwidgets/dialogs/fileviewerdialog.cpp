/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileviewerdialog.h"
#include "entities/blob.h"
#include "repository.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KParts/PartLoader>
#include <KStandardAction>
#include <KXMLGUIFactory>

#include <QFile>
#include <QMimeData>
#include <QMimeDatabase>
#include <QProgressDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QStyle>

FileViewerDialog::FileViewerDialog(const Git::Blob &file, QWidget *parent)
    : KParts::MainWindow(parent)
{
    setupUi(this);

    if (file.isBinary()) {
        stackedWidget->setCurrentIndex(3);
    } else {
        showFile(file);
    }
    QSettings s;
    restoreGeometry(s.value(QStringLiteral("FileViewerDialog_Geometry")).toByteArray());
    KStandardAction::close(this, &QMainWindow::close, actionCollection());

    // setupGUI(ToolBar, QStringLiteral("kommitfileviewerui.rc"));
}

FileViewerDialog::~FileViewerDialog()
{
    QSettings s;
    s.setValue(QStringLiteral("FileViewerDialog_Geometry"), saveGeometry());

    if (!mFilePath.isEmpty() && QFile::exists(mFilePath))
        QFile::remove(mFilePath);

    if (mPart) {
        // QProgressDialog progressDialog(this);
        // progressDialog.setWindowTitle(i18nc("@title:window", "Closing preview"));
        // progressDialog.setLabelText(i18n("Please wait while the preview is being closed…"));

        // progressDialog.setMinimumDuration(500);
        // progressDialog.setModal(true);
        // progressDialog.setCancelButton(nullptr);
        // progressDialog.setRange(0, 0);

        // #261785: this preview dialog is not modal, so we need to delete
        //          the previewed file ourselves when the dialog is closed;

        mPart.data()->closeUrl();
    }

    guiFactory()->removeClient(mPart);
    // delete mPart;
}

void FileViewerDialog::showFile(const Git::Blob &file)
{
    QMimeDatabase mimeDatabase;
    const auto fn = file.filePath().mid(file.filePath().lastIndexOf(QLatin1Char('/')) + 1);
    const auto mime = mimeDatabase.mimeTypeForFile(fn, QMimeDatabase::MatchExtension);

    plainTextEdit->setReadOnly(true);

    setWindowTitle(i18nc("@title:window", "View file: %1", file.filePath()));
    setWindowFilePath(file.filePath());
    auto icon = QIcon::fromTheme(mime.iconName()).pixmap(style()->pixelMetric(QStyle::PixelMetric::PM_SmallIconSize));
    setWindowIcon(icon);

    if (showWithParts(mime, file))
        return;

    if (mime.name().startsWith(QStringLiteral("image/")))
        showAsImage(file);
    else
        showInEditor(file);
}

void FileViewerDialog::showInEditor(const Git::Blob &file)
{
    stackedWidget->setCurrentIndex(0);
    plainTextEdit->setPlainText(file.content());
    plainTextEdit->setHighlighting(file.filePath());
}

void FileViewerDialog::showAsImage(const Git::Blob &file)
{
    stackedWidget->setCurrentIndex(1);
    mFilePath = file.saveAsTemp();
    QImage img{mFilePath};
    labelImage->setPixmap(QPixmap::fromImage(img));
}

bool FileViewerDialog::showWithParts(const QMimeType &mimeType, const Git::Blob &file)
{
    const auto result = KParts::PartLoader::instantiatePartForMimeType<KParts::ReadOnlyPart>(mimeType.name(), widgetContainer, widgetContainer);

    if (!result) {
        qDebug() << "Failed to create internal viewer";
        return false;
    }

    mPart = std::move(result.plugin);

    if (!mPart.data()) {
        qDebug() << "Failed to create internal viewer's data";
        return false;
    }

    kPartWidgetLayout->addWidget(mPart->widget());
    stackedWidget->setCurrentIndex(2);

    createGUI(mPart.data());

    auto f = file.saveAsTemp();
    mPart->openUrl(QUrl::fromLocalFile(f));
    return true;
}

void FileViewerDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();

    KParts::MainWindow::keyPressEvent(event);
}

#include "moc_fileviewerdialog.cpp"
