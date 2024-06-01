/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

// application header
#include "commandargsparser.h"
#include "dialogs/changedfilesdialog.h"

// KF headers
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>

// Qt headers
#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>

#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif

int main(int argc, char **argv)
{
#if HAVE_KICONTHEME && (KICONTHEMES_VERSION >= QT_VERSION_CHECK(6, 3, 0))
    KIconTheme::initTheme();
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication application(argc, argv);

    KLocalizedString::setApplicationDomain("kommit");
    KCrash::initialize();
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif
#endif
    KAboutData aboutData(QStringLiteral("kommit"),
                         i18n("Kommit"),
                         QStringLiteral(GK_VERSION),
                         i18n("Git client gui for KDE"),
                         KAboutLicense::GPL_V3,
                         i18n("Copyright 2021-2024, Hamed Masafi <hamed.masafi@gmail.com>"));

    aboutData.addAuthor(i18nc("@info:credit", "Hamed Masafi"), i18n("Author"), QStringLiteral("hamed.masafi@gmail.com"));
    aboutData.addCredit(i18nc("@info:credit", "Laurent Montel"), i18n("Bug fixing"), QStringLiteral("montel@kde.org"));
    aboutData.setDesktopFileName(QStringLiteral("org.kde.kommit"));

    KAboutData::setApplicationData(aboutData);
    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("kommit")));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.process(application);
    aboutData.processCommandLine(&parser);

    KDBusService appDBusService(KDBusService::Multiple | KDBusService::NoExitOnFailure);

    CommandArgsParser p;
    const auto w = p.run(application.arguments());
    if (w.type == ExecApp)
        return application.exec();

    return w.code;
}
