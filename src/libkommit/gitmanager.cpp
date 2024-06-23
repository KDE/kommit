// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gitmanager.h"

#include "entities/branch.h"
#include "entities/commit.h"
#include "entities/index.h"
#include "entities/note.h"
#include "entities/submodule.h"
#include "entities/tag.h"
#include "entities/tree.h"
#include "gitglobal_p.h"
#include "models/branchesmodel.h"
#include "models/logsmodel.h"
#include "models/remotesmodel.h"
#include "models/stashesmodel.h"
#include "models/submodulesmodel.h"
#include "models/tagsmodel.h"
#include "observers/cloneobserver.h"
#include "observers/fetchobserver.h"
#include "observers/pushobserver.h"
#include "options/addsubmoduleoptions.h"
#include "types.h"

#include "libkommit_debug.h"
#include <QFile>
#include <QProcess>
#include <QSortFilterProxyModel>
#include <QtConcurrent>

#include <abstractreference.h>
#include <git2.h>
#include <git2/branch.h>
#include <git2/diff.h>
#include <git2/errors.h>
#include <git2/refs.h>
#include <git2/stash.h>
#include <git2/submodule.h>
#include <git2/tag.h>

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

namespace Git
{

const QString &Manager::path() const
{
    return mPath;
}

bool Manager::open(const QString &newPath)
{
    if (mPath == newPath)
        return false;

    BEGIN
    STEP git_repository_open_ext(&mRepo, newPath.toUtf8().data(), 0, NULL);

    END;

    if (IS_ERROR) {
        mIsValid = false;
    } else {
        mPath = git_repository_workdir(mRepo);
        mIsValid = true;

        loadAsync();
    }

    Q_EMIT pathChanged();
    Q_EMIT reloadRequired();

    return IS_OK;
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
    const auto buffer = runGit({QStringLiteral("status"),
                                QStringLiteral("--untracked-files=all"),
                                QStringLiteral("--ignored"),
                                QStringLiteral("--short"),
                                QStringLiteral("--ignore-submodules"),
                                QStringLiteral("--porcelain")})
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

    END;
    return IS_OK;
}

bool Manager::removeRemote(const QString &name) const
{
    BEGIN
    STEP git_remote_delete(mRepo, name.toUtf8().data());
    //    runGit({QStringLiteral("remote"), QStringLiteral("remove"), name});

    END;
    return IS_OK;
}

bool Manager::renameRemote(const QString &name, const QString &newName) const
{
    git_strarray problems = {0};

    BEGIN
    STEP git_remote_rename(&problems, mRepo, name.toUtf8().data(), newName.toUtf8().data());
    git_strarray_free(&problems);

    //     runGit({QStringLiteral("remote"), QStringLiteral("rename"), name, newName});
    return IS_OK;
}

bool Manager::fetch(const QString &remoteName, FetchObserver *observer)
{
    git_remote *remote;

    BEGIN

    STEP git_remote_lookup(&remote, mRepo, remoteName.toLocal8Bit().data());

    if (IS_ERROR) {
        END;
        return false;
    }

    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;

    if (observer) {
        fetch_opts.callbacks.update_tips = &FetchObserverCallbacks::git_helper_update_tips_cb;
        fetch_opts.callbacks.sideband_progress = &FetchObserverCallbacks::git_helper_sideband_progress_cb;
        fetch_opts.callbacks.transfer_progress = &FetchObserverCallbacks::git_helper_transfer_progress_cb;
        fetch_opts.callbacks.credentials = &FetchObserverCallbacks::git_helper_credentials_cb;
        fetch_opts.callbacks.payload = observer;
    }

    STEP git_remote_fetch(remote, NULL, &fetch_opts, "fetch");
    git_remote_free(remote);

    PRINT_ERROR;

    return IS_OK;
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

    size_t ahead;
    size_t behind;
    git_reference *ref1;
    git_reference *ref2;

    BEGIN
    STEP git_branch_lookup(&ref1, mRepo, branch1.toLocal8Bit().constData(), GIT_BRANCH_LOCAL);
    STEP git_branch_lookup(&ref2, mRepo, branch2.toLocal8Bit().constData(), GIT_BRANCH_LOCAL);

    if (IS_ERROR)
        return qMakePair(0, 0);
    auto id1 = git_reference_target(ref1);
    auto id2 = git_reference_target(ref2);
    STEP git_graph_ahead_behind(&ahead, &behind, mRepo, id1, id2);

    if (IS_ERROR)
        return qMakePair(0, 0);
    PRINT_ERROR;

    return qMakePair(ahead, behind);
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

    git_tree *fromTree{nullptr};
    git_tree *toTree{nullptr};

    git_object *fromObject;
    git_commit *fromCommit = NULL;
    git_object *toObject;
    git_commit *toCommit = NULL;
    git_diff *diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;

    STEP git_revparse_single(&fromObject, mRepo, from.toLatin1().constData());
    STEP git_commit_lookup(&fromCommit, mRepo, git_object_id(fromObject));
    STEP git_commit_tree(&fromTree, fromCommit);

    if (to.isEmpty()) {
        git_diff_tree_to_workdir(&diff, mRepo, fromTree, &opts);
        RETURN_IF_ERR({});
    } else {
        STEP git_revparse_single(&toObject, mRepo, to.toLatin1().constData());
        STEP git_commit_lookup(&toCommit, mRepo, git_object_id(toObject));
        STEP git_commit_tree(&toTree, toCommit);

        RETURN_IF_ERR({});

        git_diff_tree_to_tree(&diff, mRepo, fromTree, toTree, &opts);
        git_commit_free(toCommit);
    }

    git_commit_free(fromCommit);

    git_diff_stats *stats;
    git_diff_get_stats(&stats, diff);
    auto n = git_diff_stats_files_changed(stats);
    QList<FileStatus> files2;

    for (size_t i = 0; i < n; ++i) {
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

    return files2;
}

QList<FileStatus> Manager::diff(AbstractReference *from, AbstractReference *to) const
{
    BEGIN

    git_diff *diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;

    STEP git_diff_tree_to_tree(&diff, mRepo, from->tree().data(), to->tree().data(), &opts);

    if (IS_ERROR) {
        PRINT_ERROR;
        return {};
    }

    git_diff_stats *stats;
    git_diff_get_stats(&stats, diff);
    auto n = git_diff_stats_files_changed(stats);
    QList<FileStatus> files2;

    for (size_t i = 0; i < n; ++i) {
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

    return files2;
}

void Manager::forEachCommits(std::function<void(QSharedPointer<Commit>)> callback, const QString &branchName) const
{
    if (!mRepo)
        return;

    git_revwalk *walker;
    git_commit *commit;
    git_oid oid;

    git_revwalk_new(&walker, mRepo);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);

    if (branchName.isEmpty()) {
        git_revwalk_push_head(walker);
    } else {
        git_reference *ref;
        auto n = git_branch_lookup(&ref, mRepo, branchName.toLocal8Bit().data(), GIT_BRANCH_ALL);

        if (n)
            return;

        auto refName = git_reference_name(ref);
        git_revwalk_push_ref(walker, refName);
    }

    while (!git_revwalk_next(&oid, walker)) {
        if (git_commit_lookup(&commit, mRepo, &oid)) {
            fprintf(stderr, "Failed to lookup the next object\n");
            return;
        }

        auto c = new Commit{commit};
        callback(QSharedPointer<Commit>{c});
    }

    git_revwalk_free(walker);
}

void Manager::forEachSubmodules(std::function<void(Submodule *)> callback)
{
    struct wrapper {
        std::function<void(Submodule *)> callback;
        git_repository *repo;
    };

    auto cb = [](git_submodule *sm, const char *name, void *payload) -> int {
        Q_UNUSED(name)

        auto w = reinterpret_cast<wrapper *>(payload);
        auto submodule = new Submodule{w->repo, sm};

        w->callback(submodule);
        return 0;
    };

    wrapper w;
    w.callback = callback;
    w.repo = mRepo;
    git_submodule_foreach(mRepo, cb, &w);
}

bool Manager::addSubmodule(const AddSubmoduleOptions &options) const
{
    git_submodule *submodule{nullptr};
    git_repository *submoduleRepo;
    git_submodule_update_options opts = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;

    options.applyToFetchOptions(&opts.fetch_opts);
    options.applyToCheckoutOptions(&opts.checkout_opts);

    BEGIN
    STEP git_submodule_add_setup(&submodule, mRepo, toConstChars(options.url), toConstChars(options.path), 1);
    STEP git_submodule_clone(&submoduleRepo, submodule, &opts);
    STEP git_submodule_add_finalize(submodule);
    PRINT_ERROR;

    git_submodule_free(submodule);
    return IS_OK;
}

bool Manager::removeSubmodule(const QString &name) const
{
    Q_UNUSED(name)
    return false;
}

PointerList<Submodule> Manager::submodules() const
{
    struct Data {
        PointerList<Submodule> list;
        git_repository *repo;
    };

    auto cb = [](git_submodule *sm, const char *name, void *payload) -> int {
        Q_UNUSED(name);

        auto data = reinterpret_cast<Data *>(payload);

        QSharedPointer<Submodule> submodule{new Submodule{data->repo, sm}};
        data->list.append(submodule);

        return 0;
    };

    Data data;
    data.repo = mRepo;
    git_submodule_foreach(mRepo, cb, &data);

    return data.list;
}

QSharedPointer<Submodule> Manager::submodule(const QString &name) const
{
    git_submodule *submodule{nullptr};
    BEGIN
    STEP git_submodule_lookup(&submodule, mRepo, toConstChars(name));
    PRINT_ERROR;

    if (IS_ERROR)
        return nullptr;

    return QSharedPointer<Submodule>{new Submodule{submodule}};
}

QSharedPointer<Index> Manager::index() const
{
    git_index *index;
    BEGIN
    STEP git_repository_index(&index, mRepo);
    PRINT_ERROR;

    if (IS_ERROR)
        return nullptr;

    return QSharedPointer<Index>{new Index{index}};
}

QSharedPointer<Tree> Manager::headTree() const
{
    git_reference *ref;
    git_tree *tree;

    BEGIN
    STEP git_repository_head(&ref, mRepo);
    PRINT_ERROR;
    RETURN_IF_ERR(nullptr);
    auto r = Reference{ref};
    auto oid = git_reference_target(ref);
    STEP git_tree_lookup(&tree, mRepo, oid);
    PRINT_ERROR;
    RETURN_IF_ERR(nullptr);

    return QSharedPointer<Tree>{new Tree{tree}};
}

TreeDiff Manager::diff(QSharedPointer<Tree> oldTree, QSharedPointer<Tree> newTree)
{
    git_diff *diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;
    git_diff_stats *stats;

    BEGIN

    if (newTree) {
        STEP git_diff_tree_to_tree(&diff, mRepo, oldTree->gitTree(), newTree->gitTree(), &opts);
    } else {
        STEP git_diff_tree_to_workdir(&diff, mRepo, oldTree->gitTree(), &opts);
    }
    STEP git_diff_get_stats(&stats, diff);

    if (IS_ERROR)
        return {};

    auto n = git_diff_stats_files_changed(stats);

    TreeDiff treeDiff;

    for (size_t i = 0; i < n; ++i)
        treeDiff << TreeDiffEntry{git_diff_get_delta(diff, i)};

    git_diff_free(diff);

    return treeDiff;
}

QString Manager::config(const QString &name, ConfigType type) const
{
    if (!mRepo) {
        return {};
    }
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
    int buf{};
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

    PRINT_ERROR;
    if (IS_ERROR)
        return false;

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
    git_stash_foreach(mRepo, callback, &w);

    return w.index;
}

QStringList Manager::readAllNonEmptyOutput(const QStringList &cmd) const
{
    QStringList list;
    const auto out = runGit(cmd).split(QLatin1Char('\n'));

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

    if (IS_OK)
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

QList<QSharedPointer<Note>> Manager::notes() const
{
    struct wrapper {
        git_repository *repo;
        QList<QSharedPointer<Note>> notes;
    };
    auto cb = [](const git_oid *blob_id, const git_oid *annotated_object_id, void *payload) -> int {
        Q_UNUSED(blob_id);

        auto w = reinterpret_cast<wrapper *>(payload);
        git_note *note;
        if (!git_note_read(&note, w->repo, NULL, annotated_object_id))
            w->notes.append(QSharedPointer<Note>{new Note{note}});
        return 0;
    };
    wrapper w;
    w.repo = mRepo;
    git_note_foreach(mRepo, NULL, cb, &w);
    return w.notes;
}

void Manager::forEachRefs(std::function<void(QSharedPointer<Reference>)> callback) const
{
    struct wrapper {
        std::function<void(QSharedPointer<Reference>)> cb;
    };
    auto cb = [](git_reference *reference, void *payload) -> int {
        auto w = reinterpret_cast<wrapper *>(payload);

        auto refPtr = QSharedPointer<Reference>(new Reference{reference});
        w->cb(refPtr);
        return 0;
    };

    wrapper w;
    w.cb = callback;
    git_reference_foreach(mRepo, cb, &w);
}

Manager::Manager()
    : QObject()
    , mRemotesModel{new RemotesModel(this, this)}
    , mSubmodulesModel{new SubmodulesModel(this, this)}
    , mBranchesModel{new BranchesModel(this, this)}
    , mLogsCache{new LogsModel(this, this)}
    , mStashesCache{new StashesModel(this, this)}
    , mTagsModel{new TagsModel(this, this)}
{
    git_libgit2_init();
}

Manager::Manager(git_repository *repo)
    : Manager()
{
    git_libgit2_init();
    mRepo = repo;
    mPath = git_repository_workdir(mRepo);
}

Manager::Manager(const QString &path)
    : Manager()
{
    git_libgit2_init();
    (void)open(path);
}

Manager *Manager::instance()
{
    static Manager instance;
    return &instance;
}

QString Manager::currentBranch() const
{
    if (isDetached())
        return {};

    git_reference *ref;
    BEGIN
    STEP git_repository_head(&ref, mRepo);
    if (IS_ERROR) {
        PRINT_ERROR;
        return {};
    }

    QString branchName{git_reference_shorthand(ref)};

    git_reference_free(ref);

    return branchName;
    //    const auto ret = QString(runGit({QStringLiteral("rev-parse"), QStringLiteral("--abbrev-ref"), QStringLiteral("HEAD")}))
    //                         .remove(QLatin1Char('\n'))
    //                         .remove(QLatin1Char('\r'));
    //    return ret;
}

bool Manager::createBranch(const QString &branchName) const
{
    git_reference *ref{nullptr};
    git_commit *commit{nullptr};
    git_reference *head{nullptr};

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

    return IS_OK;
}

bool Manager::switchBranch(const QString &branchName) const
{
    git_reference *branch{nullptr};
    git_object *treeish = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    BEGIN
    STEP git_branch_lookup(&branch, mRepo, branchName.toLocal8Bit().constData(), GIT_BRANCH_ALL);
    STEP git_revparse_single(&treeish, mRepo, branchName.toLocal8Bit().constData());
    STEP git_checkout_tree(mRepo, treeish, &opts);
    auto refName = git_reference_name(branch);
    STEP git_repository_set_head(mRepo, refName);

    git_reference_free(branch);
    return IS_OK;
}

QString Manager::run(const AbstractCommand &cmd) const
{
    return runGit(cmd.generateArgs());
}

bool Manager::init(const QString &path)
{
    //    runGit({QStringLiteral("init")});

    BEGIN
    git_repository_init_options initopts = {GIT_REPOSITORY_INIT_OPTIONS_VERSION, GIT_REPOSITORY_INIT_MKPATH};
    STEP git_repository_init_ext(&mRepo, path.toLatin1().data(), &initopts);

    if (IS_ERROR)
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
        opts.fetch_opts.callbacks.update_tips = &FetchObserverCallbacks::git_helper_update_tips_cb;
        opts.fetch_opts.callbacks.sideband_progress = &FetchObserverCallbacks::git_helper_sideband_progress_cb;
        opts.fetch_opts.callbacks.transfer_progress = &FetchObserverCallbacks::git_helper_transfer_progress_cb;
        opts.fetch_opts.callbacks.credentials = &FetchObserverCallbacks::git_helper_credentials_cb;
        opts.fetch_opts.callbacks.pack_progress = &FetchObserverCallbacks::git_helper_packbuilder_progress;
        opts.fetch_opts.callbacks.transport = &FetchObserverCallbacks::git_helper_transport_cb;
        // opts.checkout_opts.progress_cb = &CloneCallbacks::git_helper_checkout_progress_cb;
        // opts.checkout_opts.notify_cb = &CloneCallbacks::git_helper_checkout_notify_cb;
        // opts.checkout_opts.perfdata_cb = &CloneCallbacks::git_helper_checkout_perfdata_cb;

        observer->init(&opts.checkout_opts);

        opts.fetch_opts.callbacks.payload = observer;
    }

    BEGIN
    STEP git_clone(&mRepo, url.toLocal8Bit().constData(), localPath.toLocal8Bit().constData(), &opts);

    PRINT_ERROR;

    if (IS_ERROR)
        return false;

    mPath = git_repository_workdir(mRepo);
    return IS_OK;
}

QString Manager::runGit(const QStringList &args) const
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
        qCWarning(KOMMITLIB_LOG) << "=== Crash on git process ===";
        qCWarning(KOMMITLIB_LOG) << "====\nERROR:\n====\n" << err;
        qCWarning(KOMMITLIB_LOG) << "====\nOUTPUR:\n====\n" << out;
    }
    return QString::fromUtf8(out); // + err;
}

QStringList Manager::ls(const QString &place) const
{
    struct wrapper {
        QStringList files;
    };

    auto cb = [](const char *root, const git_tree_entry *entry, void *payload) -> int {
        auto w = reinterpret_cast<wrapper *>(payload);
        qDebug() << QString{root} << QString{git_tree_entry_name(entry)};
        w->files << QString{root} + QString{git_tree_entry_name(entry)};
        return 0;
    };
    git_tree *tree;
    git_object *placeObject{nullptr};
    git_commit *commit{nullptr};

    BEGIN
    STEP git_revparse_single(&placeObject, mRepo, place.toLatin1().constData());
    STEP git_commit_lookup(&commit, mRepo, git_object_id(placeObject));
    STEP git_commit_tree(&tree, commit);

    wrapper w;
    STEP git_tree_walk(tree, GIT_TREEWALK_PRE, cb, &w);

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

    PRINT_ERROR;

    if (IS_ERROR)
        return {};

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
    f.write(buffer.toUtf8());
    f.close();
}

QStringList Manager::branchesNames(BranchType type)
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

        qCDebug(KOMMITLIB_LOG) << git_reference_name(ref);
        const char *branchName;
        git_branch_name(&branchName, ref);
        list << branchName;
        git_reference_free(ref);
    }
    git_branch_iterator_free(it);

    return list;
}

PointerList<Branch> Manager::branches(BranchType type) const
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
    PointerList<Branch> list;
    if (!it) {
        return list;
    }
    git_reference *ref;
    git_branch_t b;
    // qDebug() << " *it " << it;
    while (!git_branch_next(&ref, &b, it)) {
        auto branch = new Branch{ref};
        list << QSharedPointer<Branch>{branch};
    }
    git_branch_iterator_free(it);

    return list;
}

void Manager::forEachTags(std::function<void(QSharedPointer<Tag>)> cb)
{
    struct wrapper {
        git_repository *repo;
        std::function<void(QSharedPointer<Tag>)> cb;
    };

    wrapper w;
    w.cb = cb;
    w.repo = mRepo;

    auto callback_c = [](const char *name, git_oid *oid_c, void *payload) {
        Q_UNUSED(name)
        auto w = reinterpret_cast<wrapper *>(payload);
        git_tag *t;

        BEGIN
        STEP git_tag_lookup(&t, w->repo, oid_c);

        if (IS_ERROR) {
            git_commit *commit;
            RESTART;
            STEP git_commit_lookup(&commit, w->repo, oid_c);

            PRINT_ERROR;
            RETURN_IF_ERR(0);

            git_reference *ref;
            STEP git_reference_lookup(&ref, w->repo, name);
            RETURN_IF_ERR(0);

            auto lightTagName = QString{git_reference_shorthand(ref)};
            QSharedPointer<Tag> tag{new Tag{commit, lightTagName}};
            w->cb(tag);
            return 0;
        }

        if (!t)
            return 0;
        QSharedPointer<Tag> tag{new Tag{t}};

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

QStringList Manager::tagsNames() const
{
    return readAllNonEmptyOutput({QStringLiteral("tag"), QStringLiteral("--list")});
}

QList<QSharedPointer<Tag>> Manager::tags() const
{
    struct wrapper {
        git_repository *repo;
        QList<QSharedPointer<Tag>> tags;
    };

    wrapper w;
    w.repo = mRepo;

    auto callback_c = [](const char *name, git_oid *oid_c, void *payload) {
        Q_UNUSED(name)
        auto w = reinterpret_cast<wrapper *>(payload);
        git_tag *t{};
        if (git_tag_lookup(&t, w->repo, oid_c))
            return 0;

        if (!t)
            return 0;

        w->tags << QSharedPointer<Tag>::create(t);

        return 0;
    };

    git_tag_foreach(mRepo, callback_c, &w);

    return w.tags;
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

    return IS_OK;
}

bool Manager::removeTag(const QString &name) const
{
    BEGIN
    STEP git_tag_delete(mRepo, name.toLocal8Bit().constData());
    PRINT_ERROR;
    return IS_OK;
}

bool Manager::removeTag(QSharedPointer<Tag> tag) const
{
    BEGIN
    STEP git_tag_delete(mRepo, tag->name().toLocal8Bit().constData());
    PRINT_ERROR;
    return IS_OK;
}

void Manager::forEachStash(std::function<void(QSharedPointer<Stash>)> cb)
{
    struct wrapper {
        Manager *manager;
        std::function<void(QSharedPointer<Stash>)> cb;
    };

    auto callback = [](size_t index, const char *message, const git_oid *stash_id, void *payload) {
        auto w = static_cast<wrapper *>(payload);

        QSharedPointer<Stash> ptr{new Stash{index, w->manager->mRepo, message, stash_id}};
        //        Stash s{index, w->manager->mRepo, message, stash_id};

        w->cb(ptr);

        return 0;
    };

    wrapper w;
    w.cb = cb;
    w.manager = this;
    git_stash_foreach(mRepo, callback, &w);
}

PointerList<Stash> Manager::stashes() const
{
    struct wrapper {
        const Manager *manager;
        PointerList<Stash> list;
    };

    auto callback = [](size_t index, const char *message, const git_oid *stash_id, void *payload) {
        auto w = static_cast<wrapper *>(payload);

        QSharedPointer<Stash> ptr{new Stash{index, w->manager->mRepo, message, stash_id}};

        w->list << ptr;

        return 0;
    };

    wrapper w;
    w.manager = this;
    git_stash_foreach(mRepo, callback, &w);
    return w.list;
}

bool Manager::applyStash(QSharedPointer<Stash> stash) const
{
    if (stash.isNull())
        return false;

    git_stash_apply_options options;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_apply(mRepo, stash->index(), &options);

    PRINT_ERROR;

    return IS_OK;
}

bool Manager::popStash(QSharedPointer<Stash> stash) const
{
    if (stash.isNull())
        return false;

    git_stash_apply_options options;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_pop(mRepo, stash->index(), &options);

    return IS_OK;
}

bool Manager::removeStash(QSharedPointer<Stash> stash) const
{
    if (stash.isNull())
        return false;

    BEGIN
    STEP git_stash_drop(mRepo, stash->index());

    return IS_OK;
}

bool Manager::createStash(const QString &name) const
{
    git_oid oid;
    git_signature *sign;

    BEGIN
    STEP git_signature_default(&sign, mRepo);
    STEP git_stash_save(&oid, mRepo, sign, name.toUtf8().data(), GIT_STASH_DEFAULT);
    return IS_OK;

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

    return IS_OK;
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

    PRINT_ERROR;

    return IS_OK;

    runGit({QStringLiteral("stash"), QStringLiteral("apply"), name});
    return true;
}

bool Manager::popStash(const QString &name) const
{
    auto stashIndex = findStashIndex(name);
    git_stash_apply_options options;

    if (stashIndex == -1)
        return false;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_pop(mRepo, stashIndex, &options);

    return IS_OK;
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
    auto ret = runGit({QStringLiteral("remote"), QStringLiteral("show"), remoteName});
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
    return IS_OK;
}

bool Manager::merge(const QString &branchName) const
{
    auto state = git_repository_state(mRepo);
    if (state != GIT_REPOSITORY_STATE_NONE) {
        fprintf(stderr, "repository is in unexpected state %d\n", state);
        return false;
    }

    git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    git_annotated_commit **annotated;
    size_t annotated_count;

    merge_opts.flags = 0;
    merge_opts.file_flags = GIT_MERGE_FILE_STYLE_DIFF3;

    checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_ALLOW_CONFLICTS;

    BEGIN
    STEP git_merge(mRepo, (const git_annotated_commit **)annotated, annotated_count, &merge_opts, &checkout_opts);

    return IS_OK;
}

Commit *Manager::commitByHash(const QString &hash) const
{
    git_commit *commit;
    git_object *commitObject;
    BEGIN
    STEP git_revparse_single(&commitObject, mRepo, hash.toLatin1().constData());
    STEP git_commit_lookup(&commit, mRepo, git_object_id(commitObject));

    PRINT_ERROR;

    if (IS_ERROR)
        return nullptr;
    return new Commit(commit);
}

PointerList<Commit> Manager::commits(const QString &branchName) const
{
    PointerList<Commit> list;

    git_revwalk *walker;
    git_commit *commit;
    git_oid oid;

    git_revwalk_new(&walker, mRepo);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);

    if (branchName.isEmpty()) {
        git_revwalk_push_head(walker);
    } else {
        git_reference *ref;
        auto n = git_branch_lookup(&ref, mRepo, branchName.toLocal8Bit().data(), GIT_BRANCH_ALL);

        if (n)
            return list;

        auto refName = git_reference_name(ref);
        git_revwalk_push_ref(walker, refName);
    }

    while (!git_revwalk_next(&oid, walker)) {
        if (git_commit_lookup(&commit, mRepo, &oid)) {
            fprintf(stderr, "Failed to lookup the next object\n");
            return list;
        }

        list << QSharedPointer<Commit>{new Commit{commit}};
    }

    git_revwalk_free(walker);
    return list;
}

BlameData Manager::blame(const File &file) // TODO: change parametere to QSharedPointer<File>
{
    git_blame *blame;
    git_blame_options options;

    BEGIN
    STEP git_blame_options_init(&options, GIT_BLAME_OPTIONS_VERSION);
    STEP git_blame_file(&blame, mRepo, file.fileName().toUtf8().data(), &options);
    END;

    PRINT_ERROR;
    RETURN_IF_ERR({});

    BlameData b;

    auto lines = file.content().split(QLatin1Char('\n'));

    auto count = git_blame_get_hunk_count(blame);
    for (size_t i = 0; i < count; ++i) {
        auto hunk = git_blame_get_hunk_byindex(blame, i);

        BlameDataRow row;
        row.commitHash = convertToString(&hunk->final_commit_id, 20);
        row.code = lines.mid(hunk->final_start_line_number, hunk->lines_in_hunk).join(QLatin1Char('\n'));
        row.log = mLogsCache->findLogByHash(row.commitHash, LogsModel::LogMatchType::BeginMatch);

        b.append(row);
    }
    git_blame_free(blame);

    return b;
}

bool Manager::revertFile(const QString &filePath) const
{
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;

    const auto s = filePath.toStdString();
    const char *paths[] = {s.c_str()};
    opts.paths.strings = (char **)paths;
    opts.paths.count = 1;

    opts.checkout_strategy = GIT_CHECKOUT_FORCE;

    BEGIN
    STEP git_checkout_head(mRepo, &opts);

    PRINT_ERROR;

    return IS_OK;
}

QMap<QString, ChangeStatus> Manager::changedFiles() const
{
    struct wrapper {
        QMap<QString, ChangeStatus> files;
    };
    auto cb = [](const char *path, unsigned int status_flags, void *payload) -> int {
        auto w = reinterpret_cast<wrapper *>(payload);

        auto status = ChangeStatus::Unknown;
        if (status_flags & GIT_STATUS_WT_NEW || status_flags & GIT_STATUS_INDEX_NEW)
            status = ChangeStatus::Added;
        else if ((status_flags & GIT_STATUS_WT_MODIFIED) || (status_flags & GIT_STATUS_INDEX_MODIFIED))
            status = ChangeStatus::Modified;
        else if ((status_flags & GIT_STATUS_WT_DELETED) || (status_flags & GIT_STATUS_INDEX_DELETED))
            status = ChangeStatus::Removed;
        else if ((status_flags & GIT_STATUS_WT_RENAMED) || (status_flags & GIT_STATUS_INDEX_RENAMED))
            status = ChangeStatus::Renamed;
        else if (status_flags & GIT_STATUS_IGNORED)
            status = ChangeStatus::Ignored;
        else
            status = ChangeStatus::Unknown;
        //        if (status_flags & GIT_STATUS_INDEX_TYPECHANGE) status = ChangeStatus::ty ;

        w->files.insert(QString{path}, status);
        return 0;
    };

    wrapper w;
    git_status_options opts;
    git_status_options_init(&opts, GIT_STATUS_OPTIONS_VERSION);
    opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    opts.flags = GIT_STATUS_OPT_DEFAULTS | GIT_STATUS_OPT_EXCLUDE_SUBMODULES
        /*GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_INCLUDE_IGNORED |
   GIT_STATUS_OPT_INCLUDE_UNMODIFIED | GIT_STATUS_OPT_EXCLUDE_SUBMODULES | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS | GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH
   | GIT_STATUS_OPT_RECURSE_IGNORED_DIRS | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX | GIT_STATUS_OPT_RENAMES_INDEX_TO_WORKDIR |
   GIT_STATUS_OPT_SORT_CASE_SENSITIVELY | GIT_STATUS_OPT_SORT_CASE_INSENSITIVELY | GIT_STATUS_OPT_RENAMES_FROM_REWRITES | GIT_STATUS_OPT_NO_REFRESH |
   GIT_STATUS_OPT_UPDATE_INDEX | GIT_STATUS_OPT_INCLUDE_UNREADABLE | GIT_STATUS_OPT_INCLUDE_UNREADABLE_AS_UNTRACKED*/
        ;

    git_status_foreach_ext(mRepo, &opts, cb, &w);

    //    git_status_foreach(_repo, cb, &w);
    return w.files;
}

void Manager::commit(const QString &message)
{
    runGit({QStringLiteral("commit"), QStringLiteral("-m"), message});
    Q_EMIT reloadRequired();
}

void Manager::push(PushObserver *observer) const
{
    git_push_options opts = GIT_PUSH_OPTIONS_INIT;
    if (observer) {
        opts.callbacks.pack_progress = &PushCallbacks::git_helper_packbuilder_progress;
        opts.callbacks.push_transfer_progress = &PushCallbacks::git_helper_push_transfer_progress_cb;
        opts.callbacks.credentials = &PushCallbacks::git_helper_credential_acquire_cb;
        opts.callbacks.certificate_check = &PushCallbacks::git_helper_transport_certificate_check_cb;
        opts.callbacks.payload = observer;
    }

    //    git_remote_lookup()
    runGit({QStringLiteral("push"), QStringLiteral("origin"), QStringLiteral("master")});
}

void Manager::addFile(const QString &file) const
{
    git_index *index{nullptr};
    git_tree *tree{nullptr};
    git_oid oid;

    BEGIN
    STEP git_repository_index(&index, mRepo);
    if (file.startsWith(QLatin1Char('/')))
        STEP git_index_add_bypath(index, toConstChars(file.mid(1)));
    else
        STEP git_index_add_bypath(index, toConstChars(file));
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

    return IS_OK;

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

int Manager::errorClass() const
{
    return mErrorClass;
}

QString Manager::errorMessage() const
{
    return mErrorMessage;
}

int Manager::errorCode() const
{
    return mErrorCode;
}

} // namespace Git

#include "moc_gitmanager.cpp"
