/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dolphinplugin.h"

#include <KFileItem>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KProcess>

#include <QMenu>

#include <git2.h>

#include "statuscache.h"

K_PLUGIN_CLASS_WITH_JSON(DolphinPlugin, "komitdolphinplugin.json")

DolphinPlugin::DolphinPlugin(QObject *parent, const QList<QVariant> &args)
    : KVersionControlPlugin{parent}
    , mCache{new StatusCache}
{
    Q_UNUSED(args);

    auto paths = QIcon::themeSearchPaths();
    paths << ":/icons" << ":/icons/hicolor";
    QIcon::setThemeSearchPaths(paths);

    git_libgit2_init();

    mMainActionGit = new QAction;
    mMainActionGit->setText(i18n("Kommit"));
    mMainActionGit->setIcon(QIcon::fromTheme(QStringLiteral("kommit")));

    mMainActionNonGit = new QAction;
    mMainActionNonGit->setText(i18n("Kommit"));
    mMainActionNonGit->setIcon(QIcon::fromTheme(QStringLiteral("kommit")));

#define f(name, text, args, icon)                                                                                                                              \
    name = new QAction{text};                                                                                                                                  \
    if (!icon.isEmpty())                                                                                                                                       \
        name->setIcon(QIcon::fromTheme(icon, QIcon{":/hicolor/scalable/actions/" + icon + ".svg"}));                                                           \
    connect(name, &QAction::triggered, this, &DolphinPlugin::name##Clicked);

    ACTIONS_FOR_EACH(f)

#undef f

    auto gitMenu = new QMenu;
    gitMenu->addAction(actionOpen);
    // gitMenu->addAction(actionPull);
    // gitMenu->addAction(actionPush);
    gitMenu->addAction(actionFetch);
    gitMenu->addAction(actionCreateTag);
    gitMenu->addSeparator();
    gitMenu->addAction(actionModifications);
    gitMenu->addAction(actionDiff);
    gitMenu->addAction(actionDiffBranches);
    gitMenu->addSeparator();
    gitMenu->addAction(actionMerge);
    gitMenu->addAction(actionSwitchCheckout);
    gitMenu->addSeparator();
    gitMenu->addAction(actionIgnoreFile);
    gitMenu->addAction(actionCleanup);
    gitMenu->addAction(actionRemove);
    gitMenu->addAction(actionBlame);
    gitMenu->addAction(actionHistory);
    mMainActionGit->setMenu(gitMenu);

    auto nonGitMenu = new QMenu;
    nonGitMenu->addAction(actionInit);
    nonGitMenu->addAction(actionClone);
    mMainActionNonGit->setMenu(nonGitMenu);
}

DolphinPlugin::~DolphinPlugin()
{
    git_libgit2_shutdown();
}

#define f(name, text, args, icon)                                                                                                                              \
    void DolphinPlugin::name##Clicked()                                                                                                                        \
    {                                                                                                                                                          \
        KProcess::startDetached(QStringLiteral("kommit"), args);                                                                                               \
    }

ACTIONS_FOR_EACH(f)
#undef f

QString DolphinPlugin::fileName() const
{
    return QLatin1String(".git");
}

bool DolphinPlugin::beginRetrieval(const QString &directory)
{
    auto isGit = mCache->setPath(directory);

    if (!isGit)
        return false;

    if (mCache->submoduleName().isEmpty())
        actionPush->setText(i18n("Push/Commit to %1", mCache->currentBranch()));
    else
        actionPush->setText(i18n("Push/Commit to submodule %1", mCache->submoduleName()));

    return true;
}

void DolphinPlugin::endRetrieval()
{
}

KVersionControlPlugin::ItemVersion DolphinPlugin::itemVersion(const KFileItem &item) const
{
    if (!item.isLocalFile())
        return ItemVersion::NormalVersion;

    return mCache->status(item.name());
}

QList<QAction *> DolphinPlugin::versionControlActions(const KFileItemList &items) const
{
    const_cast<DolphinPlugin *>(this)->mPath = items[0].url().toLocalFile();

    return QList<QAction *>() << actionPush << actionPull << mMainActionGit;
}

QList<QAction *> DolphinPlugin::outOfVersionControlActions(const KFileItemList &items) const
{
    const_cast<DolphinPlugin *>(this)->mPath = items[0].url().toLocalFile();
    return {mMainActionNonGit};
}

#include "dolphinplugin.moc"

#include "moc_dolphinplugin.cpp"
