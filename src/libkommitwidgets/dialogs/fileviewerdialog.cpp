/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileviewerdialog.h"
#include "entities/file.h"
#include "gitmanager.h"

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

FileViewerDialog::FileViewerDialog(QSharedPointer<Git::File> file, QWidget *parent)
    : KParts::MainWindow(parent)
{
    setupUi(this);
    showFile(*file.data());
    QSettings s;
    restoreGeometry(s.value(QStringLiteral("FileViewerDialog_Geometry")).toByteArray());
    KStandardAction::close(this, &QMainWindow::close, actionCollection());

    setupGUI(ToolBar, QStringLiteral("kommitfileviewerui.rc"));
}

FileViewerDialog::~FileViewerDialog()
{
    QSettings s;
    s.setValue(QStringLiteral("FileViewerDialog_Geometry"), saveGeometry());

    if (!mFilePath.isEmpty() && QFile::exists(mFilePath))
        QFile::remove(mFilePath);

    if (mPart) {
        QProgressDialog progressDialog(this);
        progressDialog.setWindowTitle(i18nc("@title:window", "Closing preview"));
        progressDialog.setLabelText(i18n("Please wait while the preview is being closed…"));

        progressDialog.setMinimumDuration(500);
        progressDialog.setModal(true);
        progressDialog.setCancelButton(nullptr);
        progressDialog.setRange(0, 0);

        // #261785: this preview dialog is not modal, so we need to delete
        //          the previewed file ourselves when the dialog is closed;

        mPart.data()->closeUrl();

        //        if (!m_fileName.isEmpty()) {
        //            QFile::remove(m_fileName);
        //        }
    }

    guiFactory()->removeClient(mPart);
    delete mPart;
}

void FileViewerDialog::showFile(const Git::File &file)
{
    QMimeDatabase mimeDatabase;
    const auto fn = file.fileName().mid(file.fileName().lastIndexOf(QLatin1Char('/')) + 1);
    const auto mime = mimeDatabase.mimeTypeForFile(fn, QMimeDatabase::MatchExtension);

    lineEditBranchName->setText(file.place());
    lineEditFileName->setText(file.fileName());
    plainTextEdit->setReadOnly(true);

    setWindowTitle(i18nc("@title:window", "View file: %1", file.fileName()));
    setWindowFilePath(file.fileName());
    labelFileIcon->setPixmap(QIcon::fromTheme(mime.iconName()).pixmap(style()->pixelMetric(QStyle::PixelMetric::PM_SmallIconSize)));

    if (showWithParts(mime, file))
        return;

    if (mime.name().startsWith(QStringLiteral("image/")))
        showAsImage(file);
    else
        showInEditor(file);
}

void FileViewerDialog::showInEditor(const Git::File &file)
{
    stackedWidget->setCurrentIndex(0);
    plainTextEdit->setPlainText(file.content());
    plainTextEdit->setHighlighting(file.fileName());
}

void FileViewerDialog::showAsImage(const Git::File &file)
{
    stackedWidget->setCurrentIndex(1);
    mFilePath = file.saveAsTemp();
    QImage img{mFilePath};
    labelImage->setPixmap(QPixmap::fromImage(img));
}

bool FileViewerDialog::showWithParts(const QMimeType &mimeType, const Git::File &file)
{
    auto parts = KParts::PartLoader::partsForMimeType(mimeType.name());

    if (parts.empty())
        return false;

    auto viewer = parts[0];
    auto icon = QIcon::fromTheme(mimeType.iconName()).pixmap(style()->pixelMetric(QStyle::PixelMetric::PM_SmallIconSize));
    setWindowIcon(icon);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const auto result = KParts::PartLoader::createPartInstanceForMimeType<KParts::ReadOnlyPart>(mimeType.name(), this, this);
#else
    const auto result = KParts::PartLoader::instantiatePart<KParts::ReadOnlyPart>(mimeType.name(), this, this);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    mPart = result;
#else
    mPart = result.plugin;
#endif
    if (!result) {
        qDebug() << "Failed to create internal viewer";
        return false;
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    kPartWidgetLayout->addWidget(result->widget());
#else
    kPartWidgetLayout->addWidget(mPart->widget());
#endif
    stackedWidget->setCurrentIndex(2);

    createGUI(mPart.data());

    auto f = file.saveAsTemp();
    mPart.data()->openUrl(QUrl::fromLocalFile(f));
    return true;
}

void FileViewerDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();

    KParts::MainWindow::keyPressEvent(event);
}

#include "moc_fileviewerdialog.cpp"
