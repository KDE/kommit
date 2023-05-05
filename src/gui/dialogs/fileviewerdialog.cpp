/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileviewerdialog.h"
#include "gitmanager.h"

#include <KActionCollection>
#include <KLocalizedString>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <KMimeTypeTrader>
#endif
#include <KStandardAction>
#include <KXMLGUIFactory>

#include "kommit_appdebug.h"
#include <QFile>
#include <QMimeData>
#include <QMimeDatabase>
#include <QProgressDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QStyle>

// TODO: This file need to be refactored
FileViewerDialog::FileViewerDialog(Git::Manager *git, const QString &place, const QString &fileName, QWidget *parent)
    : FileViewerDialog(git, Git::File(git, place, fileName), parent)
{
}

FileViewerDialog::FileViewerDialog(Git::Manager *git, const Git::File &file, QWidget *parent)
    : KParts::MainWindow(parent)
    , mGit(git)
{
    setupUi(this);
    showFile(Git::File(file));
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

    if (m_part) {
        QProgressDialog progressDialog(this);
        progressDialog.setWindowTitle(i18nc("@title:window", "Closing preview"));
        progressDialog.setLabelText(i18n("Please wait while the preview is being closed..."));

        progressDialog.setMinimumDuration(500);
        progressDialog.setModal(true);
        progressDialog.setCancelButton(nullptr);
        progressDialog.setRange(0, 0);

        // #261785: this preview dialog is not modal, so we need to delete
        //          the previewed file ourselves when the dialog is closed;

        m_part.data()->closeUrl();

        //        if (!m_fileName.isEmpty()) {
        //            QFile::remove(m_fileName);
        //        }
    }

    guiFactory()->removeClient(m_part);
    delete m_part;
}

void FileViewerDialog::showFile(const Git::File &file)
{
    QMimeDatabase mimeDatabase;
    const auto fn = file.fileName().mid(file.fileName().lastIndexOf(QLatin1Char('/')) + 1);
    const auto mime = mimeDatabase.mimeTypeForFile(fn, QMimeDatabase::MatchExtension);
    mFilePath = file.fileName();
    mFilePath = mFilePath.mid(mFilePath.lastIndexOf(QLatin1Char('/')) + 1);
    mFilePath.prepend(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1Char('/'));

    lineEditBranchName->setText(file.place());
    lineEditFileName->setText(file.fileName());
    plainTextEdit->setReadOnly(true);
    setWindowTitle(i18nc("@title:window", "View file: %1", file.fileName()));
    setWindowFilePath(file.fileName());
    labelFileIcon->setPixmap(QIcon::fromTheme(mime.iconName()).pixmap(style()->pixelMetric(QStyle::PixelMetric::PM_SmallIconSize)));

    auto ptr = getInternalViewer(mime.name());
    if (ptr && ptr->isValid()) {
        file.save(mFilePath);
        if (viewInInternalViewer(ptr, mFilePath, mime))
            return;
    }

    if (mime.name().startsWith(QStringLiteral("text/")))
        showInEditor(file);
    else if (mime.name().startsWith(QStringLiteral("image/")))
        showAsImage(file);
    else {
        if (!ptr || !ptr->isValid()) {
            showInEditor(file);
            qCDebug(KOMMIT_LOG) << "fallback to text mode";
        } else {
            file.save(mFilePath);
            if (!viewInInternalViewer(ptr, mFilePath, mime))
                showInEditor(file);
        }
    }
    qCDebug(KOMMIT_LOG) << "mime is" << mime.name() << fn << mimeDatabase.suffixForFileName(fn) << stackedWidget->currentIndex();
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
    const auto p = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/klient_img");
    file.save(p);
    QImage img{p};
    labelImage->setPixmap(QPixmap::fromImage(img));
}

KService::Ptr FileViewerDialog::getInternalViewer(const QString &mimeType)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // No point in even trying to find anything for application/octet-stream
    if (mimeType == QLatin1String("application/octet-stream")) {
        return {};
    }

    // Try to get a read-only kpart for the internal viewer
    KService::List offers = KMimeTypeTrader::self()->query(mimeType, QStringLiteral("KParts/ReadOnlyPart"));

    qCDebug(KOMMIT_LOG) << offers.size() << "offer(s) found for" << mimeType;
    for (const auto &offer : std::as_const(offers))
        qCDebug(KOMMIT_LOG) << " *" << offer->name() << offer->genericName();
    /*auto arkPartIt = std::find_if(offers.begin(), offers.end(), [](KService::Ptr service) {
        return service->storageId() == QLatin1String("ark_part.desktop");
    });

    // Use the Ark part only when the mime type matches an archive type directly.
    // Many file types (e.g. Open Document) are technically just archives
    // but browsing their internals is typically not what the user wants.
    if (arkPartIt != offers.end()) {
        // Not using hasMimeType() as we're explicitly not interested in inheritance.
        if (!(*arkPartIt)->mimeTypes().contains(mimeType)) {
            offers.erase(arkPartIt);
        }
    }*/

    // Skip the KHTML part
    auto khtmlPart = std::find_if(offers.begin(), offers.end(), [](KService::Ptr service) {
        return service->desktopEntryName() == QLatin1String("khtml");
    });

    if (khtmlPart != offers.end()) {
        offers.erase(khtmlPart);
    }

    if (!offers.isEmpty()) {
        return offers.first();
    } else {
        return {};
    }
#else
    qWarning() << "FileViewerDialog::getInternalViewer need to be port to QT6 ";
    return {};

#endif
}

void FileViewerDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();

    KParts::MainWindow::keyPressEvent(event);
}

KService::Ptr FileViewerDialog::getExternalViewer(const QString &mimeType)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qCDebug(KOMMIT_LOG) << mimeType;
    const KService::List offers = KMimeTypeTrader::self()->query(mimeType);

    if (!offers.isEmpty()) {
        return offers.first();
    } else {
        return {};
    }
#else
    qWarning() << "FileViewerDialog::getExternalViewer need to be port to QT6 ";
    return {};
#endif
}

bool FileViewerDialog::viewInInternalViewer(const KService::Ptr &viewer, const QString &fileName, const QMimeType &mimeType)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_UNUSED(mimeType)
    // Set icon and comment for the mimetype.
    //    m_iconLabel->setPixmap(QIcon::fromTheme(mimeType.iconName()).pixmap(style()->pixelMetric(QStyle::PixelMetric::PM_SmallIconSize)));
    //    m_commentLabel->setText(mimeType.comment());

    // Create the ReadOnlyPart instance.
    QString error;
    m_part = viewer->createInstance<KParts::ReadOnlyPart>(widgetContainer, widgetContainer, QVariantList(), &error);

    if (!m_part.data()) {
        qCDebug(KOMMIT_LOG) << "m_part is null" << error;
        return false;
    }

    // Insert the KPart into its placeholder.
    kPartWidgetLayout->addWidget(m_part.data()->widget());
    stackedWidget->setCurrentIndex(2);
    //    layout()->replaceWidget(plainTextEdit, m_part.data()->widget());
    /*
        QAction* action = actionCollection()->addAction(QStringLiteral("help_about_kpart"));
        const KPluginMetaData partMetaData = m_part->metaData();
        const QString iconName = partMetaData.iconName();
        if (!iconName.isEmpty()) {
            action->setIcon(QIcon::fromTheme(iconName));
        }
        action->setText(i18nc("@action", "About Viewer Component"));
        connect(action, &QAction::triggered, this, &ArkViewer::aboutKPart);
    */
    createGUI(m_part.data());
    //    setAutoSaveSettings(QStringLiteral("Viewer"), true);

    m_part.data()->openUrl(QUrl::fromLocalFile(fileName));
    m_part.data()->widget()->setFocus();
    //    m_fileName = fileName;
#else
    qWarning() << "FileViewerDialog::viewInInternalViewer need to be port to QT6 ";
    return {};
#endif
    return true;
}
