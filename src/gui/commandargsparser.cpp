
/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandargsparser.h"

#include "appwindow.h"
#include "core/kmessageboxhelper.h"
#include "dialogs/changedfilesdialog.h"
#include "dialogs/cleanupdialog.h"
#include "dialogs/clonedialog.h"
#include "dialogs/commitpushdialog.h"
#include "dialogs/fetchdialog.h"
#include "dialogs/fileblamedialog.h"
#include "dialogs/filehistorydialog.h"
#include "dialogs/ignorefiledialog.h"
#include "dialogs/initdialog.h"
#include "dialogs/mergedialog.h"
#include "dialogs/pulldialog.h"
#include "dialogs/resolvedialog.h"
#include "dialogs/runnerdialog.h"
#include "dialogs/selectbranchestodiffdialog.h"
#include "dialogs/switchbranchdialog.h"
#include "dialogs/taginfodialog.h"
#include "kommit_appdebug.h"
#include "settings/settingsmanager.h"

#include <dialogs/filestreedialog.h>
#include <windows/diffwindow.h>
#include <windows/mergewindow.h>

#include <Kommit/BranchesCache>
#include <Kommit/CommandClean>
#include <Kommit/CommandMerge>
#include <Kommit/CommandSwitchBranch>
#include <Kommit/File>
#include <Kommit/Index>
#include <Kommit/Repository>
#include <Kommit/TagsCache>
#include <Kommit/Tree>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMetaMethod>

#include <KLocalizedString>
#include <KMessageBox>

namespace Errors
{
constexpr int InvalidPath = 1;
}

CommandArgsParser::CommandArgsParser()
    : QObject()
    , mGit(new Git::Repository)
{
    SettingsManager::applyToLib();
}

CommandArgsParser::~CommandArgsParser()
{
    delete mGit;
}

bool CommandArgsParser::checkGitPath(const QString &path)
{
    QFileInfo fi(path);
    if (fi.isFile()) {
        mGit->open(fi.absolutePath());
    } else {
        mGit->open(path);
    }
    if (!mGit->isValid()) {
        KMessageBox::error(nullptr, i18n("The path is not git repo: %1", path));
        return false;
    }
    return true;
}

ArgParserReturn CommandArgsParser::run(const QStringList &args)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define GET_OP(x) params.size() > x ? Q_ARG(QString, params.at(x)) : QGenericArgument()
#else
#define GET_OP(x) params.size() > x ? Q_ARG(QString, params.at(x)) : QMetaMethodArgument()
#endif
    if (args.size() == 1)
        return main();
    const auto name = QString(args.at(1)).replace(QLatin1String("-"), QLatin1String("_")).toLocal8Bit();
    const auto c = metaObject()->methodCount();
    qCDebug(KOMMIT_LOG) << "Running" << args;
    for (int i = 0; i < c; i++) {
        const auto method = metaObject()->method(i);

        if (method.name().compare(name, Qt::CaseInsensitive) == 0) {
            if (method.parameterCount() == args.size() - 2) {
                auto params = args.mid(2);
                ArgParserReturn r;
                qCDebug(KOMMIT_LOG) << "Running:" << method.name();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                auto b = metaObject()->invokeMethod(this,
                                                    method.name().constData(),
                                                    Q_RETURN_ARG(ArgParserReturn, r),
                                                    GET_OP(0),
                                                    GET_OP(1),
                                                    GET_OP(2),
                                                    GET_OP(3),
                                                    GET_OP(4),
                                                    GET_OP(5),
                                                    GET_OP(6),
                                                    GET_OP(7),
                                                    GET_OP(8),
                                                    GET_OP(9));
#else
                bool b{};
                switch (method.parameterCount()) {
                case 0:
                    b = metaObject()->invokeMethod(this, method.name().constData(), Q_RETURN_ARG(ArgParserReturn, r));
                    break;

                case 1:
                    b = metaObject()->invokeMethod(this, method.name().constData(), Q_RETURN_ARG(ArgParserReturn, r), GET_OP(0));
                    break;

                case 2:
                    b = metaObject()->invokeMethod(this, method.name().constData(), Q_RETURN_ARG(ArgParserReturn, r), GET_OP(0), GET_OP(1));
                    break;

                case 3:
                    b = metaObject()->invokeMethod(this, method.name().constData(), Q_RETURN_ARG(ArgParserReturn, r), GET_OP(0), GET_OP(1), GET_OP(2));
                    break;

                case 4:
                    b = metaObject()
                            ->invokeMethod(this, method.name().constData(), Q_RETURN_ARG(ArgParserReturn, r), GET_OP(0), GET_OP(1), GET_OP(2), GET_OP(3));
                    break;
                }
#endif

                if (!b) {
                    qCDebug(KOMMIT_LOG) << args.size() << method.parameterCount();
                }

                return r;
            }
        }
    }
#undef GET_OP

    if (args.size() == 2)
        return main(args.at(1));

    qWarning().noquote() << "Method not found" << args.at(1);
    return main();
}

ArgParserReturn CommandArgsParser::help()
{
    const auto c = metaObject()->methodCount();

    for (auto i = metaObject()->classInfoOffset(); i < metaObject()->classInfoCount(); i++) {
        auto name = QString(metaObject()->classInfo(i).name());
        const auto value = QString(metaObject()->classInfo(i).value());

        if (!name.startsWith(QStringLiteral("help.")))
            continue;
        name = name.mid(5);

        mHelpTexts.insert(name, value);
    }
    qCDebug(KOMMIT_LOG) << "Git Klient command line interface help:";
    for (int i = metaObject()->methodOffset(); i < c; i++) {
        auto method = metaObject()->method(i);
        qCDebug(KOMMIT_LOG).noquote() << "    " << method.name() << method.parameterNames().join(" ");
        qCDebug(KOMMIT_LOG).noquote() << mHelpTexts.value(method.name());
    }
    return ArgParserReturn{0};
}

ArgParserReturn CommandArgsParser::clone()
{
    CloneDialog d{mGit};

    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r(mGit);

        auto cmd = d.command();
        r.run(cmd);
        r.exec();
        cmd->deleteLater();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::clone(const QString &path)
{
    CloneDialog d{mGit};
    d.setLocalPath(path);

    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r(mGit);

        auto cmd = d.command();
        r.run(cmd);
        r.exec();
        cmd->deleteLater();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::init(const QString &path)
{
    InitDialog d(mGit);
    d.setPath(path);

    if (d.exec() == QDialog::Accepted) {
        QDir dir;
        if (!dir.mkpath(d.path())) {
            KMessageBox::error(nullptr, i18n("Unable to create path: %1", d.path()), i18n("Init repo"));
            return 1;
        }

        mGit->init(d.path());
        KMessageBox::information(nullptr, i18n("The repo inited successfully"));
    }
    return 0;
}

ArgParserReturn CommandArgsParser::pull(const QString &path)
{
    mGit->open(path);
    PullDialog d(mGit);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::fetch(const QString &path)
{
    mGit->open(path);
    FetchDialog d(mGit);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::push(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    CommitPushDialog d(mGit);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::merge(const QString &path)
{
    mGit->open(path);

    if (!mGit->isValid())
        return Errors::InvalidPath;

    MergeDialog d(mGit);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r(mGit);

        auto cmd = d.command();
        r.run(cmd);
        r.exec();
        cmd->deleteLater();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::changes()
{
    QDir dir;
    mGit->open(dir.currentPath());
    ChangedFilesDialog d(mGit);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::changes(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    ChangedFilesDialog d(mGit);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::create_tag(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    TagInfoDialog d(nullptr);

    if (d.exec() == QDialog::Accepted) {
        mGit->tags()->create(d.tagName(), d.message());
    }
    return 0;
}

ArgParserReturn CommandArgsParser::diff()
{
    auto d = new DiffWindow();
    d->exec();
    return ExecApp;
}

ArgParserReturn CommandArgsParser::diff(const QString &file)
{
    QFileInfo fi{file};

    if (fi.isFile()) {
        mGit->open(fi.absolutePath());
        const QDir dir{mGit->path()};

        auto headFile = Git::Blob::lookup(mGit->repoPtr(), mGit->branches()->currentName(), dir.relativeFilePath(file));
        auto changedFile = Git::Blob::fromDisk(mGit->repoPtr(), file);

        auto d = new DiffWindow{headFile, changedFile};
        d->exec();
        return ExecApp;
    } else if (fi.isDir()) {
        mGit->open(fi.absoluteFilePath());
        auto current = mGit->branches()->current();
        auto d = new DiffWindow{mGit, &current};
        d->exec();
        return ExecApp;
    }
    return 0;
}

ArgParserReturn CommandArgsParser::diff(const QString &file1, const QString &file2)
{
    qCDebug(KOMMIT_LOG) << file1 << file2;
    QFileInfo fi1(file1);
    QFileInfo fi2(file2);

    if (fi1.isFile() && fi2.isFile()) {
        auto d = new DiffWindow(DiffWindow::Files, file1, file2);
        d->exec();
        return ExecApp;
    }
    if (fi1.isDir() && fi2.isDir()) {
        auto d = new DiffWindow(DiffWindow::Dirs, fi1.absoluteFilePath(), fi2.absoluteFilePath());
        d->exec();
        return ExecApp;
    }

    return 0;
}

ArgParserReturn CommandArgsParser::diff(const QString &path, const QString &file1, const QString &file2)
{
    if (file1.count(QLatin1Char(':')) != 1 || file2.count(QLatin1Char(':')) != 1)
        return 1;
    mGit->open(path);
    if (!mGit->isValid())
        return 1;
    const auto parts1 = file1.split(QLatin1Char(':'));
    const auto parts2 = file2.split(QLatin1Char(':'));

    auto blob1 = Git::Blob::lookup(mGit->repoPtr(), parts1.first(), parts1.at(1));
    auto blob2 = Git::Blob::lookup(mGit->repoPtr(), parts2.first(), parts2.at(1));
    auto d = new DiffWindow(blob1, blob2);
    d->exec();
    return ExecApp;
}

ArgParserReturn CommandArgsParser::blame(const QString &file)
{
    QFileInfo fi{file};

    if (!fi.exists() || !fi.isFile()) {
        return 0;
    }

    mGit->open(fi.absolutePath());
    auto fileName = file.mid(mGit->path().size());

    FileBlameDialog d(mGit, fileName);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::history(const QString &file)
{
    QFileInfo fi{file};

    if (!fi.exists()) {
        KMessageBox::error(nullptr, i18n("Cannot find the file"), i18n("History"));
        return 0;
    }
    mGit->open(fi.absolutePath());
    auto fileCopy = file.mid(mGit->path().size());
    FileHistoryDialog d(mGit, fileCopy);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::merge()
{
    auto d = new MergeWindow(mGit);
    d->exec();
    return ExecApp;
}

ArgParserReturn CommandArgsParser::merge(const QString &base, const QString &local, const QString &remote, const QString &result)
{
    auto d = new MergeWindow(mGit);
    d->setLocalFile(local);
    d->setBaseFile(base);
    d->setRemoteFile(remote);
    d->setResultFile(result);
    d->compare();
    int n = d->exec();

    if (n == QDialog::Accepted)
        return 0;
    else
        return 1;
}

ArgParserReturn CommandArgsParser::ignore(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.exists())
        return 1;

    if (fi.isDir())
        mGit->open(path);
    else
        mGit->open(fi.absolutePath());

    if (!mGit->isValid())
        return 1;

    IgnoreFileDialog d(mGit, path);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::cleanup(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    CleanupDialog d;
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner(mGit);
        runner.run(d.command());
        runner.exec();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::switch_checkout(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    if (mGit->isMerging()) {
        KMessageBox::error(nullptr, i18n("Cannot switch branch while merging"), i18n("Switch branch"));
        return 1;
    }
    SwitchBranchDialog d(mGit);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner(mGit);
        runner.run(d.command());
        runner.exec();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::diff_branches(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    SelectBranchesToDiffDialog d(mGit);
    if (d.exec() == QDialog::Accepted) {
        auto leftBranch = mGit->branches()->findByName(d.oldBranch());
        auto rightBranch = mGit->branches()->findByName(d.newBranch());

        auto diffWin = new DiffWindow(mGit, &leftBranch, &rightBranch);
        diffWin->exec();
        return 0;
    }
    return 1;
}

ArgParserReturn CommandArgsParser::browse(const QString &place)
{
    if (!checkGitPath(QDir::currentPath())) {
        return 1;
    }

    FilesTreeDialog d{mGit, place};
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::browse(const QString &path, const QString &place)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    FilesTreeDialog d{mGit, place};
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::conflicts(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    ResolveDialog d{mGit};
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::add(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    auto index = mGit->index();
    index.addByPath(path);

    if (index.writeTree())
        KMessageBox::information(nullptr, i18n("File(s) added to git successfully"));

    return 0;
}

ArgParserReturn CommandArgsParser::remove(const QString &path)
{
    if (!checkGitPath(path)) {
        return 1;
    }

    auto cached = KMessageBoxHelper::removeQuestion(nullptr, i18n("Would you like to leave file(s) on disk?"), i18nc("@title:window", "Remove from index"));
    mGit->removeFile(path, cached);
    KMessageBox::information(nullptr, i18n("File(s) removed from git successfully"));

    return 0;
}

ArgParserReturn CommandArgsParser::main()
{
    auto window = AppWindow::instance();
    window->show();
    return ExecApp;
}

ArgParserReturn CommandArgsParser::main(const QString &path)
{
    auto window = new AppWindow(path);
    window->show();
    return ExecApp;
}

#include "moc_commandargsparser.cpp"
