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
    mMainAction = new QAction;
    mMainAction->setText(i18n("Git Klient"));
    mMainAction->setIcon(QIcon::fromTheme(QStringLiteral("gitklient")));

    auto menu = new QMenu;

#define f(name, text, args, icon)                                                                                                                              \
    name = menu->addAction(i18n(text));                                                                                                                        \
    if (!icon.isEmpty())                                                                                                                                       \
        name->setIcon(QIcon::fromTheme(icon));                                                                                                                 \
    connect(name, &QAction::triggered, this, &ActionManager::name##Clicked);

    ACTIONS_FOR_EACH(f)
#undef f

    mMainAction->setMenu(menu);
}

#define f(name, text, args, icon)                                                                                                                              \
    void ActionManager::name##Clicked()                                                                                                                        \
    {                                                                                                                                                          \
        KProcess::startDetached(QStringLiteral("gitklient"), args);                                                                                            \
    }

ACTIONS_FOR_EACH(f)
#undef f

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

    auto items = fileItemInfos.items();
    bool isGit{false};
    bool isFile{false};

    actionAdd->setVisible(false);
    actionRemove->setVisible(false);
    if (items.size() == 1) {
        KFileItem &item = items.first();
        const auto path = item.url().toLocalFile();
        mPath = path;
        FileStatus::Status status;
        if (item.isFile())
            status = mCache.fileStatus(path);
        else
            status = mCache.pathStatus(path);

        if (status == FileStatus::NoGit) {
        } else {
            actionAdd->setVisible(status == FileStatus::Untracked);
            actionRemove->setVisible(status != FileStatus::Untracked);
        }
        isGit = status != FileStatus::NoGit;
        isFile = item.isFile();
    }

    actionClone->setVisible(!isGit);
    actionInit->setVisible(!isGit);

    actionOpen->setVisible(isGit);
    actionPull->setVisible(isGit);
    actionPush->setVisible(isGit);
    actionMerge->setVisible(isGit);
    actionModifications->setVisible(isGit);
    actionDiff->setVisible(isGit);
    actionIgnoreFile->setVisible(isGit);
    actionHistory->setVisible(isFile);
    actionBlame->setVisible(isFile);
    actionCleanup->setVisible(isGit);
    actionDiffBranches->setVisible(isGit);
    actionSwitchCheckout->setVisible(isGit);

    actionCreateTag->setVisible(isGit);
    actionCleanup->setVisible(isGit);

    return QList<QAction *>() << mMainAction;
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

K_PLUGIN_CLASS_WITH_JSON(ActionManager, "gitklientitemaction.json")
#include "actionmanager.moc"
