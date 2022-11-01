/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "settingsmanager.h"

#include "GitKlientSettings.h"
#include "gitklient_appdebug.h"
#include "gitmanager.h"

#include <QCalendar>

#include <KConfigDialog>

#include <QWidget>

SettingsManager::SettingsManager(Git::Manager *git, QWidget *parentWidget)
    : QObject(parentWidget)
    , mGit(git)
    , mParentWidget(parentWidget)
{
}

void SettingsManager::settingsChanged()
{
    GitKlientSettings::setCalendarType(pageBase.kcfg_calendarTypeIndex->currentText());
    GitKlientSettings::self()->save();

    mGit->setConfig(QStringLiteral("http.proxy"), pageGit.lineEditGitProxy->text(), Git::Manager::ConfigGlobal);

    if (GitKlientSettings::registerDiffTool()) {
        mGit->setConfig(QStringLiteral("difftool.gitklientdiff.cmd"), QStringLiteral("gitklientdiff \"$LOCAL\" \"$REMOTE\""), Git::Manager::ConfigGlobal);
    } else {
        mGit->unsetConfig(QStringLiteral("difftool.gitklientdiff.cmd"), Git::Manager::ConfigGlobal);
    }
    if (GitKlientSettings::registerMergeTool()) {
        mGit->setConfig(QStringLiteral("mergetool.gitklientmerge.cmd"),
                        QStringLiteral("gitklientmerge \"$BASE\" \"$LOCAL\" \"$REMOTE\" \"$MERGED\""),
                        Git::Manager::ConfigGlobal);
        mGit->setConfig(QStringLiteral("mergetool.gitklientmerge.trustExitCode"), QStringLiteral("true"), Git::Manager::ConfigGlobal);
    } else {
        mGit->unsetConfig(QStringLiteral("mergetool.gitklientmerge.cmd"), Git::Manager::ConfigGlobal);
        mGit->unsetConfig(QStringLiteral("mergetool.gitklientmerge.trustExitCode"), Git::Manager::ConfigGlobal);
    }
}

void SettingsManager::show()
{
    exec(mParentWidget);
}

QWidget *SettingsManager::createBasePage()
{
    auto w = new QWidget;
    pageBase.setupUi(w);

    auto availableCalendars = QCalendar::availableCalendars();
    std::sort(availableCalendars.begin(), availableCalendars.end());
    pageBase.kcfg_calendarTypeIndex->addItems(availableCalendars);
    pageBase.kcfg_calendarTypeIndex->setCurrentText(GitKlientSettings::calendarType());
    return w;
}
QWidget *SettingsManager::createDiffPage()
{
    auto w = new QWidget;
    pageDiff.setupUi(w);
    return w;
}
QWidget *SettingsManager::createGitPage()
{
    auto w = new QWidget;
    pageGit.setupUi(w);
    pageGit.lineEditGitProxy->setText(mGit->config(QStringLiteral("http.proxy"), Git::Manager::ConfigGlobal));
    return w;
}

void SettingsManager::exec(QWidget *parentWidget)
{
    const auto name = QStringLiteral("settings");
    auto dialog = KConfigDialog::exists(name);

    if (dialog) {
        dialog->setParent(parentWidget);
        dialog->exec();
        return;
    }

    dialog = new KConfigDialog(parentWidget, name, GitKlientSettings::self());

    dialog->addPage(createBasePage(), i18n("General"), QStringLiteral("package_setting"));
    dialog->addPage(createDiffPage(), i18n("Diff"), QStringLiteral("package_setting"));
    dialog->addPage(createGitPage(), i18n("Git"), QStringLiteral("package_setting"));

    connect(dialog, &KConfigDialog::settingsChanged, this, &SettingsManager::settingsChanged);
    dialog->show();
}
