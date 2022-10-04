/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "actionmanager.h"

#include <KFileItem>
#include <KFileItemListProperties>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KProcess>
#include <QAction>
#include <QMenu>

ActionManager::ActionManager(QObject *parent, const QList<QVariant> &)
    : KAbstractFileItemActionPlugin(parent)
{
}

void ActionManager::addMenu(QMenu *menu, const QString &title, const QStringList &args, const QString &icon)
{
    auto action = menu->addAction(title);
    if (!icon.isEmpty())
        action->setIcon(QIcon::fromTheme(icon));
    connect(action, &QAction::triggered, [args]() {
        KProcess::startDetached(QStringLiteral("gitklient"), args);
    });
}

QList<QAction *> ActionManager::actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);

    auto mainAction = new QAction;
    mainAction->setText(i18n("Git Klient"));
    mainAction->setIcon(QIcon::fromTheme(QStringLiteral("gitklient")));

    auto menu = new QMenu;

    auto items = fileItemInfos.items();
    if (items.size() == 1) {
        KFileItem &item = items.first();
        auto path = item.url().toLocalFile();
        FileStatus::Status status;
        if (item.isFile())
            status = mCache.fileStatus(path);
        else
            status = mCache.pathStatus(path);

        if (status == FileStatus::NoGit) {
            if (fileItemInfos.isDirectory()) {
                addMenuToNonGitFile(menu, path);
            }
        } else {
            addMenuToGitFile(menu, path, fileItemInfos.isFile(), status);
        }
    }

    mainAction->setMenu(menu);
    /*
        auto openAction = new QAction;
        openAction->setText("Open git klient");
        openAction->setIcon(QIcon::fromTheme("gitklient"));
    */
    return QList<QAction *>() << /*openAction << */ mainAction;
}

QString ActionManager::getCommonPart(const KFileItemList &fileItems)
{
    if (!fileItems.size())
        return {};

    QStringList list;
    for (auto const &i : fileItems)
        list.append(i.url().toLocalFile());

    QString root = list.front();
    for (QStringList::const_iterator it = list.cbegin(); it != list.cend(); ++it) {
        if (root.length() > it->length()) {
            root.truncate(it->length());
        }

        for (int i = 0; i < root.length(); ++i) {
            if (root.at(i) != it->at(i)) {
                root.truncate(i);
                break;
            }
        }
    }

    return root;
}

void ActionManager::addMenuToNonGitFile(QMenu *menu, const QString &path)
{
    addMenu(menu, i18n("Clone"), {QStringLiteral("clone"), path});
    addMenu(menu, i18n("Init"), {QStringLiteral("init"), path});
}

void ActionManager::addMenuToGitFile(QMenu *menu, const QString &path, bool isFile, const FileStatus::Status &status)
{
    addMenu(menu, i18n("Open"), {path});
    addMenu(menu, i18n("Pull"), {QStringLiteral("pull"), path}, QStringLiteral("git-pull"));
    addMenu(menu, i18n("Push"), {QStringLiteral("push"), path}, QStringLiteral("git-push"));
    addMenu(menu, i18n("Merge"), {QStringLiteral("merge"), path}, QStringLiteral("git-merge"));
    addMenu(menu, i18n("Modifications"), {QStringLiteral("changes"), path}, QStringLiteral("gitklient-changedfiles"));
    addMenu(menu, i18n("Diff"), {QStringLiteral("diff"), path});
    addMenu(menu, i18n("Ignore file"), {QStringLiteral("ignore"), path}, QStringLiteral("git-ignore"));
    if (isFile) {
        addMenu(menu, i18n("History"), {QStringLiteral("history"), path});
        addMenu(menu, i18n("Blame"), {QStringLiteral("blame"), path});
        if (status == FileStatus::Untracked) {
            addMenu(menu, i18n("Add"), {QStringLiteral("add"), path});
        } else {
            addMenu(menu, i18n("Remove"), {QStringLiteral("remove"), path});
        }
    }
    addMenu(menu, i18n("Create tag"), {QStringLiteral("create-tag"), path});
}

K_PLUGIN_FACTORY_WITH_JSON(GitKlientPluginActionFactory, "gitklientitemaction.json", registerPlugin<ActionManager>();)
#include "actionmanager.moc"
