/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "settingsmanager.h"

#include "KommitSettings.h"
#include "kommitwidgetsglobaloptions.h"
#include "repository.h"

#include <QCalendar>

#include <KConfigDialog>

#include <QWidget>

SettingsManager::SettingsManager(Git::Repository *git, QWidget *parentWidget)
    : QObject(parentWidget)
    , mGit(git)
    , mParentWidget(parentWidget)
{
}

void SettingsManager::settingsChanged()
{
    KommitSettings::setCalendarType(pageBase.kcfg_calendarTypeIndex->currentText());
    KommitSettings::self()->save();

    auto globalConfig = Git::Repository::globalConfig();

    globalConfig.set(QStringLiteral("http.proxy"), pageGit.lineEditGitProxy->text());

    if (KommitSettings::registerDiffTool()) {
        globalConfig.set(QStringLiteral("difftool.kommitdiff.cmd"), QStringLiteral("kommitdiff \"$LOCAL\" \"$REMOTE\""));
    } else {
        globalConfig.remove(QStringLiteral("difftool.kommitdiff.cmd"));
    }
    if (KommitSettings::registerMergeTool()) {
        globalConfig.set(QStringLiteral("mergetool.kommitmerge.cmd"), QStringLiteral("kommitmerge \"$BASE\" \"$LOCAL\" \"$REMOTE\" \"$MERGED\""));
        globalConfig.set(QStringLiteral("mergetool.kommitmerge.trustExitCode"), QStringLiteral("true"));
    } else {
        globalConfig.remove(QStringLiteral("mergetool.kommitmerge.cmd"));
        globalConfig.remove(QStringLiteral("mergetool.kommitmerge.trustExitCode"));
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
    auto config = Git::Repository::globalConfig();
    pageGit.lineEditGitProxy->setText(config.valueString(QStringLiteral("http.proxy")));
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
