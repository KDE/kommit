/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "settingsmanager.h"

#include "KommitSettings.h"
#include "gitmanager.h"
#include "kommitwidgetsglobaloptions.h"

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
    KommitSettings::setCalendarType(pageBase.kcfg_calendarTypeIndex->currentText());
    KommitSettings::self()->save();

    mGit->setConfig(QStringLiteral("http.proxy"), pageGit.lineEditGitProxy->text(), Git::Manager::ConfigGlobal);

    if (KommitSettings::registerDiffTool()) {
        mGit->setConfig(QStringLiteral("difftool.kommitdiff.cmd"), QStringLiteral("kommitdiff \"$LOCAL\" \"$REMOTE\""), Git::Manager::ConfigGlobal);
    } else {
        mGit->unsetConfig(QStringLiteral("difftool.kommitdiff.cmd"), Git::Manager::ConfigGlobal);
    }
    if (KommitSettings::registerMergeTool()) {
        mGit->setConfig(QStringLiteral("mergetool.kommitmerge.cmd"),
                        QStringLiteral("kommitmerge \"$BASE\" \"$LOCAL\" \"$REMOTE\" \"$MERGED\""),
                        Git::Manager::ConfigGlobal);
        mGit->setConfig(QStringLiteral("mergetool.kommitmerge.trustExitCode"), QStringLiteral("true"), Git::Manager::ConfigGlobal);
    } else {
        mGit->unsetConfig(QStringLiteral("mergetool.kommitmerge.cmd"), Git::Manager::ConfigGlobal);
        mGit->unsetConfig(QStringLiteral("mergetool.kommitmerge.trustExitCode"), Git::Manager::ConfigGlobal);
    }

    applyToLib();
    Q_EMIT settingsUpdated();
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
    pageBase.kcfg_calendarTypeIndex->setCurrentText(KommitSettings::calendarType());
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

    dialog = new KConfigDialog(parentWidget, name, KommitSettings::self());

    dialog->addPage(createBasePage(), i18n("General"), QStringLiteral("package_setting"));
    dialog->addPage(createDiffPage(), i18n("Diff"), QStringLiteral("package_setting"));
    dialog->addPage(createGitPage(), i18n("Git"), QStringLiteral("package_setting"));

    connect(dialog, &KConfigDialog::settingsChanged, this, &SettingsManager::settingsChanged);
    dialog->show();
}

void SettingsManager::applyToLib()
{
    auto opt = KommitWidgetsGlobalOptions::instance();
    auto set = KommitSettings::self();
    opt->setCalendar(QCalendar{set->calendarType()});
    opt->setColor(Git::ChangeStatus::Added, set->diffAddedColor());
    opt->setColor(Git::ChangeStatus::Modified, set->diffModifiedColor());
    opt->setColor(Git::ChangeStatus::Removed, set->diffRemovedColor());
}

#include "moc_settingsmanager.cpp"
