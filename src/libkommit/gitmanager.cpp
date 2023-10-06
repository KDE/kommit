// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gitmanager.h"

#include "branch.h"
#include "commit.h"
#include "gitsubmodule.h"
#include "gittag.h"
#include "models/authorsmodel.h"
#include "models/branchesmodel.h"
#include "models/logsmodel.h"
#include "models/remotesmodel.h"
#include "models/stashesmodel.h"
#include "models/submodulesmodel.h"
#include "models/tagsmodel.h"
#include "observers/cloneobserver.h"
#include "observers/fetchobserver.h"
#include "types.h"

#include "libkommit_debug.h"
#include <QFile>
#include <QProcess>
#include <QSortFilterProxyModel>
#include <QtConcurrent>

#include <git2.h>
#include <git2/branch.h>
#include <git2/refs.h>
#include <git2/stash.h>
#include <git2/tag.h>

#define BEGIN int err = 0;
#define STEP err = err ? err:
#define END                                                                                                                                                    \
    do {                                                                                                                                                       \
        if (err)                                                                                                                                               \
            qDebug() << "Error" << Q_FUNC_INFO << err << ":" << gitErrorMessage(err);                                                                          \
    } while (false)

#define PRINT_ERROR                                                                                                                                            \
    do {                                                                                                                                                       \
        if (err)                                                                                                                                               \
            qDebug() << "Error" << Q_FUNC_INFO << err << ":" << gitErrorMessage(err);                                                                          \
    } while (false)

#define THROW                                                                                                                                                  \
    do {                                                                                                                                                       \
        if (!err)                                                                                                                                              \
            throw new Exception{err, gitErrorMessage(err)};                                                                                                    \
    } while (false)

namespace Git
{

namespace
{
QString gitErrorMessage(int error)
{
    const git_error *lg2err;

    if (!error)
        return {};

    if ((lg2err = git_error_last()) != NULL && lg2err->message != NULL) {
        return lg2err->message;
    }

    return {};
}

}

const QString &Manager::path() const
{
    return mPath;
}

void Manager::setPath(const QString &newPath)
{
    if (mPath == newPath)
        return;

    int n = git_repository_open_ext(&mRepo, newPath.toUtf8().data(), 0, NULL);
    check_lg2(n);

    if (n) {
        mIsValid = false;
    } else {
        mPath = git_repository_workdir(mRepo);
        mIsValid = true;

        loadAsync();
    }

    Q_EMIT pathChanged();
}

QMap<QString, ChangeStatus> Manager::changedFiles(const QString &hash) const
{
    QMap<QString, ChangeStatus> statuses;
    auto buffer = QString(runGit({QStringLiteral("show"), QStringLiteral("--name-status"), hash})).split(QLatin1Char('\n'));

    for (auto &line : buffer) {
        if (!line.trimmed().size())
            continue;

        const auto parts = line.split(QLatin1Char('\t'));
        if (parts.size() != 2)
            continue;

        const auto partFirst{parts.first()};
        if (partFirst == QLatin1Char('A'))
            statuses.insert(parts.at(1), ChangeStatus::Added);
        else if (partFirst == QLatin1Char('M'))
            statuses.insert(parts.at(1), ChangeStatus::Modified);
        else if (partFirst == QLatin1Char('D'))
            statuses.insert(parts.at(1), ChangeStatus::Removed);
        else
            qCDebug(KOMMITLIB_LOG) << "Unknown file status" << partFirst;
    }
    return statuses;
}

QStringList Manager::ignoredFiles() const
{
    return readAllNonEmptyOutput({QStringLiteral("check-ignore"), QStringLiteral("*")});
}

QList<FileStatus> Manager::repoFilesStatus() const
{
    const auto buffer = QString(runGit({QStringLiteral("status"),
                                        QStringLiteral("--untracked-files=all"),
                                        QStringLiteral("--ignored"),
                                        QStringLiteral("--short"),
                                        QStringLiteral("--ignore-submodules"),
                                        QStringLiteral("--porcelain")}))
                            .split(QLatin1Char('\n'));
    QList<FileStatus> files;
    for (const auto &item : buffer) {
        if (!item.trimmed().size())
            continue;
        FileStatus fs;
        fs.parseStatusLine(item);
        qCDebug(KOMMITLIB_LOG) << "[STATUS]" << fs.name() << fs.status();
        fs.setFullPath(mPath + QLatin1Char('/') + fs.name());
        files.append(fs);
    }
    return files;
}

bool Manager::isValid() const
{
    return mIsValid;
}

bool Manager::addRemote(const QString &name, const QString &url) const
{
    git_remote *remote;
    BEGIN
    STEP git_remote_create(&remote, mRepo, name.toUtf8().data(), url.toUtf8().data());
    //    runGit({QStringLiteral("remote"), QStringLiteral("add"), name, url});

    PRINT_ERROR;
    return !err;
}

bool Manager::removeRemote(const QString &name) const
{
    BEGIN
    STEP git_remote_delete(mRepo, name.toUtf8().data());
    //    runGit({QStringLiteral("remote"), QStringLiteral("remove"), name});

    PRINT_ERROR;
    return !err;
}

bool Manager::renameRemote(const QString &name, const QString &newName) const
{
    git_strarray problems = {0};

    BEGIN
    STEP git_remote_rename(&problems, mRepo, name.toUtf8().data(), newName.toUtf8().data());
    git_strarray_free(&problems);

    //     runGit({QStringLiteral("remote"), QStringLiteral("rename"), name, newName});
    return !err;
}

bool Manager::fetch(const QString &remoteName, FetchObserver *observer)
{
    git_remote *remote;

    BEGIN

    STEP git_remote_lookup(&remote, mRepo, remoteName.toLocal8Bit().data());

    if (err) {
        PRINT_ERROR;
        return false;
    }

    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;

    if (observer) {
        fetch_opts.callbacks.update_tips = &git_helper_update_tips_cb;
        fetch_opts.callbacks.sideband_progress = &git_helper_sideband_progress_cb;
        fetch_opts.callbacks.transfer_progress = &git_helper_transfer_progress_cb;
        fetch_opts.callbacks.credentials = &git_helper_credentials_cb;
        fetch_opts.callbacks.payload = observer;
    }

    git_strarray ref;

    STEP git_remote_fetch(remote, NULL, &fetch_opts, "fetch");
    git_remote_free(remote);

    PRINT_ERROR;

    return !err;
}

bool Manager::isIgnored(const QString &path)
{
    auto tmp = readAllNonEmptyOutput({QStringLiteral("check-ignore"), path});
    qCDebug(KOMMITLIB_LOG) << Q_FUNC_INFO << tmp;
    return !tmp.empty();
}

QPair<int, int> Manager::uniqueCommitsOnBranches(const QString &branch1, const QString &branch2) const
{
    if (branch1 == branch2)
        return qMakePair(0, 0);

    auto ret = readAllNonEmptyOutput(
        {QStringLiteral("rev-list"), QStringLiteral("--left-right"), QStringLiteral("--count"), branch1 + QStringLiteral("...") + branch2});

    if (ret.size() != 1)
        return qMakePair(-1, -1);

    const auto parts = ret.first().split(QLatin1Char('\t'));
    if (parts.size() != 2)
        return qMakePair(-1, -1);

    return qMakePair(parts.at(0).toInt(), parts.at(1).toInt());
}

QStringList Manager::fileLog(const QString &fileName) const
{
    return readAllNonEmptyOutput({QStringLiteral("log"), QStringLiteral("--format=format:%H"), QStringLiteral("--"), fileName});
}

QString Manager::diff(const QString &from, const QString &to) const
{
    return runGit({QStringLiteral("diff"), from, to});
}

QList<FileStatus> Manager::diffBranch(const QString &from) const
{
    const QStringList buffer = QString(runGit({QStringLiteral("diff"), from, QStringLiteral("--name-status")})).split(QLatin1Char('\n'));
    QList<FileStatus> files;
    for (const auto &item : buffer) {
        if (!item.trimmed().size())
            continue;
        const auto parts = item.split(QStringLiteral("\t"));
        if (parts.size() != 2)
            continue;

        FileStatus fs;
        fs.setStatus(parts.at(0));
        fs.setName(parts.at(1));
        files.append(fs);
    }
    return files;
}

QList<FileStatus> Manager::diffBranches(const QString &from, const QString &to) const
{
    BEGIN

    git_tree *fromTree;
    git_tree *toTree;

    git_object *obj;
    git_commit *a = NULL;
    STEP git_revparse_single(&obj, mRepo, from.toLatin1().constData());
    STEP git_commit_lookup(&a, mRepo, git_object_id(obj));
    git_commit_tree(&fromTree, a);
    git_commit_free(a);

    git_object *obj2;
    git_commit *a2 = NULL;
    STEP git_revparse_single(&obj2, mRepo, to.toLatin1().constData());
    STEP git_commit_lookup(&a2, mRepo, git_object_id(obj2));
    git_commit_tree(&toTree, a2);
    git_commit_free(a2);

    git_diff *diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;

    git_diff_tree_to_tree(&diff, mRepo, fromTree, toTree, &opts);
    git_diff_stats *stats;
    git_diff_get_stats(&stats, diff);
    auto n = git_diff_stats_files_changed(stats);
    QList<FileStatus> files2;

    for (int i = 0; i < n; ++i) {
        auto delta = git_diff_get_delta(diff, i);
        FileStatus fs;
        fs.mName = delta->new_file.path;

        switch (delta->status) {
        case GIT_DELTA_UNMODIFIED:
            fs.mStatus = FileStatus::Unmodified;
            break;
        case GIT_DELTA_ADDED:
            fs.mStatus = FileStatus::Added;
            break;
        case GIT_DELTA_DELETED:
            fs.mStatus = FileStatus::Removed;
            break;
        case GIT_DELTA_MODIFIED:
            fs.mStatus = FileStatus::Modified;
            break;
        case GIT_DELTA_RENAMED:
            fs.mStatus = FileStatus::Renamed;
            break;
        case GIT_DELTA_COPIED:
            fs.mStatus = FileStatus::Copied;
            break;
        case GIT_DELTA_IGNORED:
            fs.mStatus = FileStatus::Ignored;
            break;
        case GIT_DELTA_UNTRACKED:
            fs.mStatus = FileStatus::Untracked;
            break;
        case GIT_DELTA_TYPECHANGE:
            fs.mStatus = FileStatus::Unknown;
            break;
        case GIT_DELTA_UNREADABLE:
            fs.mStatus = FileStatus::Unknown;
            break;
        case GIT_DELTA_CONFLICTED:
            fs.mStatus = FileStatus::Unknown;
            break;
        }
        files2 << fs;
    }

    const auto buffer = QString(runGit({QStringLiteral("diff"), from + QStringLiteral("..") + to, QStringLiteral("--name-status")})).split(QLatin1Char('\n'));
    QList<FileStatus> files;
    for (const auto &item : buffer) {
        if (!item.trimmed().size())
            continue;
        const auto parts = item.split(QLatin1Char('\t'));
        if (parts.size() != 2)
            continue;

        FileStatus fs;
        fs.setStatus(parts.at(0));
        fs.setName(parts.at(1));
        files.append(fs);
    }
    return files2;
}

void Manager::forEachSubmodules(std::function<void(Submodule *)> callback)
{
    struct wrapper {
        std::function<void(Submodule *)> callback;
    };

    auto cb = [](git_submodule *sm, const char *name, void *payload) -> int {
        Q_UNUSED(name)

        auto w = reinterpret_cast<wrapper *>(payload);
        auto submodule = new Submodule{sm};

        w->callback(submodule);
        return 0;
    };

    wrapper w;
    w.callback = callback;
    git_submodule_foreach(mRepo, cb, &w);
}

QString Manager::config(const QString &name, ConfigType type) const
{
    BEGIN
    git_config *cfg;
    switch (type) {
    case ConfigLocal:
        STEP git_repository_config(&cfg, mRepo);
        break;
    case ConfigGlobal:
        STEP git_config_open_default(&cfg);
        break;
    }
    git_config_entry *entry = NULL;
    STEP git_config_get_entry(&entry, cfg, name.toLatin1().data());
    PRINT_ERROR;

    if (!entry)
        return {};

    QString s = entry->value;
    git_config_entry_free(entry);
    return s;
}

bool Manager::configBool(const QString &name, ConfigType type) const
{
    BEGIN
    int buf;
    git_config *cfg;
    switch (type) {
    case ConfigLocal:
        STEP git_repository_config(&cfg, mRepo);
        break;
    case ConfigGlobal:
        STEP git_config_open_default(&cfg);
        break;
    }
    STEP git_config_get_bool(&buf, cfg, name.toLatin1().data());

    return buf;
}

void Manager::setConfig(const QString &name, const QString &value, ConfigType type) const
{
    BEGIN

    git_config *cfg;
    switch (type) {
    case ConfigLocal:
        STEP git_repository_config(&cfg, mRepo);
        break;
    case ConfigGlobal:
        STEP git_config_open_default(&cfg);
        break;
    }
    STEP git_config_set_string(cfg, name.toLatin1().data(), value.toLatin1().data());

    return;

    QStringList cmd;
    switch (type) {
    case ConfigLocal:
        cmd = QStringList{QStringLiteral("config"), name, value};
        break;
    case ConfigGlobal:
        cmd = QStringList{QStringLiteral("config"), QStringLiteral("--global"), name, value};
        break;
    }

    runGit(cmd);
}

void Manager::unsetConfig(const QString &name, ConfigType type) const
{
    BEGIN

    git_config *cfg;
    switch (type) {
    case ConfigLocal:
        STEP git_repository_config(&cfg, mRepo);
        break;
    case ConfigGlobal:
        STEP git_config_open_default(&cfg);
        break;
    }
    STEP git_config_delete_entry(cfg, name.toLatin1().data());

    return;

    QStringList cmd{QStringLiteral("config"), QStringLiteral("--unset")};

    if (type == ConfigGlobal)
        cmd.append(QStringLiteral("--global"));

    cmd.append(name);

    runGit(cmd);
}

void Manager::forEachConfig(std::function<void(QString, QString)> calback)
{
    struct wrapper {
        std::function<void(QString, QString)> calback;
    };
    wrapper w;
    w.calback = calback;
    git_config *cfg = nullptr;

    BEGIN
    STEP git_config_open_default(&cfg);

    auto cb = [](const git_config_entry *entry, void *payload) -> int {
        auto w = reinterpret_cast<wrapper *>(payload);
        w->calback(QString{entry->name}, QString{entry->value});
        return 0;
    };

    git_config_foreach(cfg, cb, &w);
    git_config_free(cfg);
}

int Manager::findStashIndex(const QString &message) const
{
    struct wrapper {
        int index{-1};
        QString name;
    };
    wrapper w;
    w.name = message;
    auto callback = [](size_t index, const char *message, const git_oid *stash_id, void *payload) {
        Q_UNUSED(stash_id)
        auto w = reinterpret_cast<wrapper *>(payload);
        if (message == w->name)
            w->index = index;
        return 0;
    };
    git_stash_foreach(mRepo, callback, NULL);

    return w.index;
}

QStringList Manager::readAllNonEmptyOutput(const QStringList &cmd) const
{
    QStringList list;
    const auto out = QString(runGit(cmd)).split(QLatin1Char('\n'));

    for (const auto &line : out) {
        const auto b = line.trimmed();
        if (b.isEmpty())
            continue;

        list.append(b.trimmed());
    }
    return list;
}

QString Manager::escapeFileName(const QString &filePath) const
{
    if (filePath.contains(QLatin1Char(' ')))
        return QLatin1Char('\'') + filePath + QLatin1Char('\'');
    return filePath;
}

bool load(AbstractGitItemsModel *cache)
{
    cache->load();
    return true;
}

void Manager::loadAsync()
{
    QList<AbstractGitItemsModel *> models;
    if (mAuthorsModel) {
        mAuthorsModel->clear();
    }
    if (mLoadFlags & LoadStashes)
        models << mStashesCache;
    if (mLoadFlags & LoadRemotes)
        models << mRemotesModel;
    if (mLoadFlags & LoadSubmodules)
        models << mSubmodulesModel;
    if (mLoadFlags & LoadBranches)
        models << mBranchesModel;
    if (mLoadFlags & LoadLogs)
        models << mLogsCache;
    if (mLoadFlags & LoadTags)
        models << mTagsModel;

    if (!models.empty()) {
#ifdef QT_CONCURRENT_LIB
        QtConcurrent::mapped(models, load);
#else
        for (auto &m : models)
            m->load();
#endif
    }
}

TagsModel *Manager::tagsModel() const
{
    return mTagsModel;
}

StashesModel *Manager::stashesModel() const
{
    return mStashesCache;
}

LogsModel *Manager::logsModel() const
{
    return mLogsCache;
}

BranchesModel *Manager::branchesModel() const
{
    return mBranchesModel;
}

AuthorsModel *Manager::authorsModel() const
{
    return mAuthorsModel;
}

SubmodulesModel *Manager::submodulesModel() const
{
    return mSubmodulesModel;
}

RemotesModel *Manager::remotesModel() const
{
    return mRemotesModel;
}

const LoadFlags &Manager::loadFlags() const
{
    return mLoadFlags;
}

void Manager::setLoadFlags(Git::LoadFlags newLoadFlags)
{
    mLoadFlags = newLoadFlags;
}

Branch *Manager::branch(const QString &branchName) const
{
    git_reference *ref;
    BEGIN
    STEP git_branch_lookup(&ref, mRepo, branchName.toLocal8Bit().constData(), GIT_BRANCH_ALL);

    if (!err)
        return new Branch{ref};

    return nullptr;
}

QString Manager::readNote(const QString &branchName) const
{
    return runGit({QStringLiteral("notes"), QStringLiteral("show"), branchName});
}

void Manager::saveNote(const QString &branchName, const QString &note) const
{
    runGit({QStringLiteral("notes"), QStringLiteral("add"), branchName, QStringLiteral("-f"), QStringLiteral("--message=") + note});
}

Manager::Manager()
    : QObject()
    , mRemotesModel{new RemotesModel(this)}
    , mAuthorsModel{new AuthorsModel(this)}
    , mSubmodulesModel{new SubmodulesModel(this)}
    , mBranchesModel{new BranchesModel(this)}
    , mLogsCache{new LogsModel(this, mAuthorsModel)}
    , mStashesCache{new StashesModel(this)}
    , mTagsModel{new TagsModel(this)}
{
    git_libgit2_init();
}

Manager::Manager(const QString &path)
    : Manager()
{
    git_libgit2_init();
    setPath(path);
}

Manager *Manager::instance()
{
    static Manager instance;
    return &instance;
}

QString Manager::currentBranch() const
{
    const auto ret = QString(runGit({QStringLiteral("rev-parse"), QStringLiteral("--abbrev-ref"), QStringLiteral("HEAD")}))
                         .remove(QLatin1Char('\n'))
                         .remove(QLatin1Char('\r'));
    return ret;
}

bool Manager::createBranch(const QString &branchName) const
{
    git_reference *ref;
    git_commit *commit;
    git_reference *head;

    BEGIN
    STEP git_repository_head(&head, mRepo);

    if (!head)
        return false;

    auto targetId = git_reference_target(head);
    STEP git_commit_lookup(&commit, mRepo, targetId);
    STEP git_branch_create(&ref, mRepo, branchName.toLocal8Bit().constData(), commit, 0);

    git_reference_free(ref);
    git_reference_free(head);
    git_commit_free(commit);

    PRINT_ERROR;

    return !err;
}

bool Manager::switchBranch(const QString &branchName) const
{
    git_reference *branch;
    git_object *treeish = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    BEGIN
    STEP git_branch_lookup(&branch, mRepo, branchName.toLocal8Bit().constData(), GIT_BRANCH_ALL);
    STEP git_revparse_single(&treeish, mRepo, branchName.toLocal8Bit().constData());
    STEP git_checkout_tree(mRepo, treeish, &opts);
    auto refName = git_reference_name(branch);
    STEP git_repository_set_head(mRepo, refName);

    return !err;
}

QString Manager::run(const AbstractCommand &cmd) const
{
    return QString(runGit(cmd.generateArgs()));
}

bool Manager::init(const QString &path)
{
    //    runGit({QStringLiteral("init")});

    BEGIN
    git_repository_init_options initopts = {GIT_REPOSITORY_INIT_OPTIONS_VERSION, GIT_REPOSITORY_INIT_MKPATH};
    STEP git_repository_init_ext(&mRepo, path.toLatin1().data(), &initopts);

    if (err)
        return false;

    mPath = path;
    mIsValid = true;
    return true;
}

bool Manager::clone(const QString &url, const QString &localPath, CloneObserver *observer)
{
    // TODO: free _repo
    git_clone_options opts = GIT_CLONE_OPTIONS_INIT;

    if (observer) {
        if (observer) {
            opts.fetch_opts.callbacks.update_tips = &git_helper_update_tips_cb;
            opts.fetch_opts.callbacks.sideband_progress = &git_helper_sideband_progress_cb;
            opts.fetch_opts.callbacks.transfer_progress = &git_helper_transfer_progress_cb;
            opts.fetch_opts.callbacks.credentials = &git_helper_credentials_cb;
            opts.checkout_opts.progress_cb = &CloneCallbacks::git_helper_checkout_progress_cb;
            opts.checkout_opts.notify_cb = &CloneCallbacks::git_helper_checkout_notify_cb;
            opts.checkout_opts.perfdata_cb = &CloneCallbacks::git_helper_checkout_perfdata_cb;

            opts.fetch_opts.callbacks.payload = opts.checkout_opts.progress_payload = opts.checkout_opts.notify_payload = opts.checkout_opts.perfdata_payload =
                observer;
        }
    }

    BEGIN
    STEP git_clone(&mRepo, url.toLocal8Bit().constData(), localPath.toLocal8Bit().constData(), &opts);

    if (err)
        return false;

    mPath = git_repository_workdir(mRepo);
    return !err;
}

QByteArray Manager::runGit(const QStringList &args) const
{
    //    qCDebug(KOMMITLIB_LOG).noquote() << "Running: git " << args.join(" ");

    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments(args);
    p.setWorkingDirectory(mPath);
    p.start();
    p.waitForFinished();
    auto out = p.readAllStandardOutput();
    auto err = p.readAllStandardError();

    if (p.exitStatus() == QProcess::CrashExit) {
        qWarning() << "=== Crash on git process ===";
        qWarning() << "====\nERROR:\n====\n" << err;
        qWarning() << "====\nOUTPUR:\n====\n" << out;
    }
    return out; // + err;
}

QStringList Manager::ls(const QString &place) const
{
    //    auto cb = [](const char *root, const git_tree_entry *entry, void *payload) -> int {

    //        return 0;
    //    };
    //    const git_tree *tree;
    //    git_tree_walk(tree, GIT_TREEWALK_PRE, cb, NULL);

    auto buffer = readAllNonEmptyOutput({QStringLiteral("ls-tree"), QStringLiteral("--name-only"), QStringLiteral("-r"), place});
    QMutableListIterator<QString> it(buffer);
    while (it.hasNext()) {
        auto s = it.next();
        if (s.startsWith(QLatin1String("\"")) && s.endsWith(QLatin1String("\"")))
            it.setValue(s.mid(1, s.length() - 2));
    }
    return buffer;
}

QString Manager::fileContent(const QString &place, const QString &fileName) const
{
    git_object *placeObject{nullptr};
    git_commit *commit{nullptr};
    git_tree *tree{nullptr};
    git_tree_entry *entry{nullptr};
    git_blob *blob{nullptr};

    BEGIN
    STEP git_revparse_single(&placeObject, mRepo, place.toLatin1().constData());
    STEP git_commit_lookup(&commit, mRepo, git_object_id(placeObject));
    STEP git_commit_tree(&tree, commit);

    STEP git_tree_entry_bypath(&entry, tree, fileName.toLocal8Bit().constData());
    STEP git_blob_lookup(&blob, mRepo, git_tree_entry_id(entry));

    QString ch = (char *)git_blob_rawcontent(blob);

    git_object_free(placeObject);
    git_commit_free(commit);
    git_blob_free(blob);
    git_tree_entry_free(entry);
    git_tree_free(tree);

    //    auto ch2 = runGit({QStringLiteral("show"), place + QLatin1Char(':') + fileName});
    //    auto eq = ch == ch2;
    return ch;
}

void Manager::saveFile(const QString &place, const QString &fileName, const QString &localFile) const
{
    auto buffer = runGit({QStringLiteral("show"), place + QLatin1Char(':') + fileName});
    QFile f{localFile};
    if (!f.open(QIODevice::WriteOnly))
        return;
    f.write(buffer);
    f.close();
}

QStringList Manager::branches(BranchType type)
{
    git_branch_iterator *it;
    switch (type) {
    case BranchType::AllBranches:
        git_branch_iterator_new(&it, mRepo, GIT_BRANCH_ALL);
        break;
    case BranchType::LocalBranch:
        git_branch_iterator_new(&it, mRepo, GIT_BRANCH_LOCAL);
        break;
    case BranchType::RemoteBranch:
        git_branch_iterator_new(&it, mRepo, GIT_BRANCH_REMOTE);
        break;
    }
    git_reference *ref;
    git_branch_t b;

    QStringList list;
    while (!git_branch_next(&ref, &b, it)) {
        //        if (git_branch_is_head(ref))
        //            continue;

        qDebug() << git_reference_name(ref);
        const char *branchName;
        git_branch_name(&branchName, ref);
        list << branchName;
        git_reference_free(ref);
    }
    git_branch_iterator_free(it);

    return list;
}

void Manager::forEachTags(std::function<void(Tag *)> cb)
{
    struct wrapper {
        git_repository *repo;
        std::function<void(Tag *)> cb;
    };

    wrapper w;
    w.cb = cb;
    w.repo = mRepo;

    auto callback_c = [](const char *name, git_oid *oid_c, void *payload) {
        Q_UNUSED(name)
        auto w = reinterpret_cast<wrapper *>(payload);
        git_tag *t;
        git_tag_lookup(&t, w->repo, oid_c);

        if (!t)
            return 0;
        auto tag = new Tag{t};

        w->cb(tag);

        return 0;
    };

    git_tag_foreach(mRepo, callback_c, &w);
}

QStringList Manager::remotes() const
{
    git_strarray list{};
    git_remote_list(&list, mRepo);
    auto r = convert(&list);
    git_strarray_free(&list);
    return r;
}

QStringList Manager::tags() const
{
    return readAllNonEmptyOutput({QStringLiteral("tag"), QStringLiteral("--list")});
}

bool Manager::createTag(const QString &name, const QString &message) const
{
    git_object *target = NULL;
    git_oid oid;
    git_signature *sign;

    BEGIN
    STEP git_signature_default(&sign, mRepo);
    STEP git_revparse_single(&target, mRepo, "HEAD^{commit}");
    STEP git_tag_create(&oid, mRepo, name.toLatin1().data(), target, sign, message.toUtf8().data(), 0);

    // check_lg2(err);
    //     runGit({QStringLiteral("tag"), QStringLiteral("-a"), name, QStringLiteral("--message"), message});
    PRINT_ERROR;

    return !err;
}

bool Manager::removeTag(const QString &name) const
{
    BEGIN
    STEP git_tag_delete(mRepo, name.toLocal8Bit().constData());
    PRINT_ERROR;
    return !err;
}

void Manager::forEachStash(std::function<void(Stash *)> cb)
{
    struct wrapper {
        Manager *manager;
        std::function<void(Stash *)> cb;
    };

    auto callback = [](size_t index, const char *message, const git_oid *stash_id, void *payload) {
        auto w = static_cast<wrapper *>(payload);
        Stash s{index, w->manager->mRepo, message, stash_id};
        w->cb(&s);

        return 0;
    };

    wrapper w;
    w.cb = cb;
    w.manager = this;
    git_stash_foreach(mRepo, callback, &w);
}

QList<Stash> Manager::stashes()
{
    QList<Stash> ret;
    const auto list = readAllNonEmptyOutput({QStringLiteral("stash"), QStringLiteral("list"), QStringLiteral("--format=format:%s%m%an%m%ae%m%aD")});
    int id{0};
    for (const auto &item : std::as_const(list)) {
        const auto parts = item.split(QStringLiteral(">"));
        if (parts.size() != 4)
            continue;

        const auto subject = parts.first();
        Stash stash(this, QStringLiteral("stash@{%1}").arg(id));

        stash.mSubject = subject;
        stash.mAuthorName = parts.at(1);
        stash.mAuthorEmail = parts.at(2);
        stash.mPushTime = QDateTime::fromString(parts.at(3), Qt::RFC2822Date);
        qCDebug(KOMMITLIB_LOG) << item << subject << stash.mPushTime;

        ret.append(stash);
        id++;
    }
    return ret;
}

bool Manager::createStash(const QString &name) const
{
    git_oid oid;
    git_signature *sign;

    BEGIN
    STEP git_signature_default(&sign, mRepo);
    STEP git_stash_save(&oid, mRepo, sign, name.toUtf8().data(), GIT_STASH_DEFAULT);
    return !err;

    QStringList args{QStringLiteral("stash"), QStringLiteral("push")};

    if (!name.isEmpty())
        args.append({QStringLiteral("--message"), name});

    const auto ret = runGit(args);
    qCDebug(KOMMITLIB_LOG) << ret;
}

bool Manager::removeStash(const QString &name) const
{
    auto stashIndex = findStashIndex(name);

    if (stashIndex == -1)
        return false;

    BEGIN
    STEP git_stash_drop(mRepo, stashIndex);

    return !err;
}

bool Manager::applyStash(const QString &name) const
{
    auto stashIndex = findStashIndex(name);
    git_stash_apply_options options;

    if (stashIndex == -1)
        return false;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_apply(mRepo, stashIndex, &options);

    return !err;

    runGit({QStringLiteral("stash"), QStringLiteral("apply"), name});
    return true;
}

Remote *Manager::remote(const QString &name) const
{
    git_remote *remote;
    if (!git_remote_lookup(&remote, mRepo, name.toLocal8Bit().data()))
        return new Remote{remote};

    return nullptr;
}

Remote Manager::remoteDetails(const QString &remoteName)
{
    if (mRemotes.contains(remoteName))
        return mRemotes.value(remoteName);
    Remote r;
    auto ret = QString(runGit({QStringLiteral("remote"), QStringLiteral("show"), remoteName}));
    r.parse(ret);
    mRemotes.insert(remoteName, r);
    return r;
}

bool Manager::removeBranch(const QString &branchName) const
{
    git_reference *ref;

    BEGIN
    STEP git_branch_lookup(&ref, mRepo, branchName.toUtf8().data(), GIT_BRANCH_LOCAL);
    STEP git_branch_delete(ref);
    //    auto ret = readAllNonEmptyOutput({QStringLiteral("branch"), QStringLiteral("-D"), branchName});
    return !err;
}

BlameData Manager::blame(const File &file)
{
    git_blame *blame;
    git_blame_options options;

    BEGIN
    STEP git_blame_options_init(&options, GIT_BLAME_OPTIONS_VERSION);
    STEP git_blame_file(&blame, mRepo, file.fileName().toUtf8().data(), &options);
    END;

    if (err)
        return {};

    BlameData b;

    auto lines = file.content().split('\n');

    auto count = git_blame_get_hunk_count(blame);
    for (size_t i = 0; i < count; ++i) {
        auto hunk = git_blame_get_hunk_byindex(blame, i);

        BlameDataRow row;
        row.commitHash = convertToString(&hunk->final_commit_id, 20);
        row.code = lines.mid(hunk->final_start_line_number, hunk->lines_in_hunk).join('\n');
        row.log = mLogsCache->findLogByHash(row.commitHash, LogsModel::LogMatchType::BeginMatch);

        b.append(row);
    }
    git_blame_free(blame);

    //    const auto lines = readAllNonEmptyOutput({QStringLiteral("--no-pager"), QStringLiteral("blame"), QStringLiteral("-l"), file.fileName()});
    //    b.reserve(lines.size());

    //    for (const auto &line : lines) {
    //        BlameDataRow row;
    //        row.commitHash = line.mid(0, 40);

    //        auto metaIndex = line.indexOf(QLatin1Char(')'));
    //        row.code = line.mid(metaIndex + 1);

    //        auto hash = row.commitHash;
    //        if (hash.startsWith(QLatin1Char('^')))
    //            hash = hash.remove(0, 1);
    //        row.log = mLogsCache->findLogByHash(hash, LogsModel::LogMatchType::BeginMatch);

    //        b.append(row);
    //    }

    return b;
}

bool Manager::revertFile(const QString &filePath) const
{
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    addToArray(&opts.paths, filePath);
    opts.checkout_strategy = GIT_CHECKOUT_FORCE;

    BEGIN
    STEP git_checkout_tree(mRepo, NULL, &opts);

    PRINT_ERROR;

    return !err;
    runGit({QStringLiteral("checkout"), QStringLiteral("--"), filePath});
}

QMap<QString, ChangeStatus> Manager::changedFiles() const
{
    struct wrapper {
        QMap<QString, ChangeStatus> files;
    };
    auto cb = [](const char *path, unsigned int status_flags, void *payload) -> int {
        auto w = reinterpret_cast<wrapper *>(payload);

        ChangeStatus status;
        if (status_flags & GIT_STATUS_WT_NEW || status_flags & GIT_STATUS_INDEX_NEW)
            status = ChangeStatus::Added;
        else if (status_flags & GIT_STATUS_WT_MODIFIED)
            status = ChangeStatus::Modified;
        else if (status_flags & GIT_STATUS_WT_DELETED)
            status = ChangeStatus::Removed;
        else if (status_flags & GIT_STATUS_WT_RENAMED)
            status = ChangeStatus::Renamed;
        //        else if (status_flags & GIT_STATUS_CONFLICTED)
        //            status = ChangeStatus::UpdatedButInmerged;
        else
            status = ChangeStatus::Unknown;
        //        if (status_flags & GIT_STATUS_INDEX_TYPECHANGE) status = ChangeStatus::ty ;

        qDebug() << QString{path} << (int)status << status_flags;
        w->files.insert(QString{path}, status);
        return 0;
    };

    wrapper w;
    git_status_options opts;
    git_status_options_init(&opts, GIT_STATUS_OPTIONS_VERSION);
    opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    opts.flags = GIT_STATUS_OPT_DEFAULTS /*GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_INCLUDE_IGNORED |
          GIT_STATUS_OPT_INCLUDE_UNMODIFIED | GIT_STATUS_OPT_EXCLUDE_SUBMODULES | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS | GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH
          | GIT_STATUS_OPT_RECURSE_IGNORED_DIRS | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX | GIT_STATUS_OPT_RENAMES_INDEX_TO_WORKDIR |
          GIT_STATUS_OPT_SORT_CASE_SENSITIVELY | GIT_STATUS_OPT_SORT_CASE_INSENSITIVELY | GIT_STATUS_OPT_RENAMES_FROM_REWRITES | GIT_STATUS_OPT_NO_REFRESH |
          GIT_STATUS_OPT_UPDATE_INDEX | GIT_STATUS_OPT_INCLUDE_UNREADABLE | GIT_STATUS_OPT_INCLUDE_UNREADABLE_AS_UNTRACKED*/
        ;

    git_status_foreach_ext(mRepo, &opts, cb, &w);

    //    git_status_foreach(_repo, cb, &w);
    return w.files;

    // status --untracked-files=all --ignored --short --ignore-submodules --porcelain
    QMap<QString, ChangeStatus> statuses;
    const auto buffer = QString(runGit({QStringLiteral("status"), QStringLiteral("--short")})).split(QLatin1Char('\n'));

    for (const auto &line : buffer) {
        if (!line.trimmed().size())
            continue;

        auto status0 = line.mid(0, 1).trimmed();
        auto status1 = line.mid(1, 1).trimmed();
        const auto fileName = line.mid(3);

        if (status1 == QLatin1Char('M') || status0 == QLatin1Char('M'))
            statuses.insert(fileName, ChangeStatus::Modified);
        else if (status1 == QLatin1Char('A'))
            statuses.insert(fileName, ChangeStatus::Added);
        else if (status1 == QLatin1Char('D') || status0 == QLatin1Char('D'))
            statuses.insert(fileName, ChangeStatus::Removed);
        else if (status1 == QLatin1Char('R'))
            statuses.insert(fileName, ChangeStatus::Renamed);
        else if (status1 == QLatin1Char('C'))
            statuses.insert(fileName, ChangeStatus::Copied);
        else if (status1 == QLatin1Char('U'))
            statuses.insert(fileName, ChangeStatus::UpdatedButInmerged);
        else if (status1 == QLatin1Char('?'))
            statuses.insert(fileName, ChangeStatus::Untracked);
        else if (status1 == QLatin1Char('!'))
            statuses.insert(fileName, ChangeStatus::Ignored);
        else {
            qDebug() << __FUNCTION__ << "The status" << status1 << "is unknown";
            statuses.insert(fileName, ChangeStatus::Unknown);
        }
    }
    return statuses;
}

void Manager::commit(const QString &message) const
{
    runGit({QStringLiteral("commit"), QStringLiteral("-m"), message});
}

void Manager::push() const
{
    runGit({QStringLiteral("push"), QStringLiteral("origin"), QStringLiteral("master")});
}

void Manager::addFile(const QString &file) const
{
    git_index *index{nullptr};
    git_tree *tree{nullptr};
    git_oid oid;

    BEGIN
    STEP git_repository_index(&index, mRepo);
    STEP git_index_add_bypath(index, file.toLocal8Bit().constData());
    STEP git_index_write_tree_to(&oid, index, mRepo);
    STEP git_tree_lookup(&tree, mRepo, &oid);
    STEP git_index_write(index);

    PRINT_ERROR;

    git_tree_free(tree);
    git_index_free(index);
}

bool Manager::removeFile(const QString &file, bool cached) const
{
    git_index *index = nullptr;
    git_oid oid;
    git_tree *tree{nullptr};

    BEGIN
    STEP git_repository_index(&index, mRepo);
    STEP git_index_read(index, true);
    STEP git_index_remove_bypath(index, file.toLocal8Bit().constData());
    STEP git_index_write_tree(&oid, index);
    STEP git_tree_lookup(&tree, mRepo, &oid);
    // STEP git_index_write(index);
    PRINT_ERROR;

    git_tree_free(tree);
    git_index_free(index);

    PRINT_ERROR;

    return !err;

    QStringList args;
    args.append(QStringLiteral("rm"));
    if (cached)
        args.append(QStringLiteral("--cached"));
    args.append(file);
    runGit(args);
}

bool Manager::isMerging() const
{
    auto state = git_repository_state(mRepo);

    return state == GIT_REPOSITORY_STATE_MERGE;
}

bool Manager::isRebasing() const
{
    auto state = git_repository_state(mRepo);

    return state == GIT_REPOSITORY_STATE_REBASE || state == GIT_REPOSITORY_STATE_REBASE_INTERACTIVE || state == GIT_REPOSITORY_STATE_REBASE_MERGE;
}

bool Manager::isDetached() const
{
    return git_repository_head_detached(mRepo) == 1;
}

void Manager::commitsForEach()
{
#define GIT_SUCCESS 0

    git_oid oid;
    git_revwalk *walker;
    git_commit *commit;

    git_revwalk_new(&walker, mRepo);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);
    git_revwalk_push(walker, &oid);

    while (git_revwalk_next(&oid, walker) == GIT_SUCCESS) {
        if (git_commit_lookup(&commit, mRepo, &oid)) {
            fprintf(stderr, "Failed to lookup the next object\n");
            return;
        }

        auto d = new Commit{commit};

        git_commit_free(commit);
    }

    git_revwalk_free(walker);
}

void Manager::check_lg2(int error)
{
    const git_error *lg2err;
    const char *lg2msg = "", *lg2spacer = "";

    if (!error)
        return;

    if ((lg2err = git_error_last()) != NULL && lg2err->message != NULL) {
        lg2msg = lg2err->message;
        lg2spacer = " - ";
        qDebug() << "Error" << lg2err->message;
    }

    //    if (extra)
    //        fprintf(stderr, "%s '%s' [%d]%s%s\n",
    //                message, extra, error, lg2spacer, lg2msg);
    //    else
    //        fprintf(stderr, "%s [%d]%s%s\n",
    //                message, error, lg2spacer, lg2msg);

    //    exit(1);
}

} // namespace Git

#include "branch.h"
#include "gitsubmodule.h"
#include "moc_gitmanager.cpp"
