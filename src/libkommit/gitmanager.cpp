// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gitmanager.h"

#include "abstractreference.h"
#include "blamedata.h"
#include "caches/abstractcache.h"
#include "caches/branchescache.h"
#include "caches/commitscache.h"
#include "caches/notescache.h"
#include "caches/referencecache.h"
#include "caches/remotescache.h"
#include "caches/stashescache.h"
#include "caches/submodulescache.h"
#include "caches/tagscache.h"
#include "commands/abstractcommand.h"
#include "entities/branch.h"
#include "entities/commit.h"
#include "entities/index.h"
#include "entities/note.h"
#include "entities/submodule.h"
#include "entities/tree.h"
#include "entities/treediff.h"
#include "filestatus.h"
#include "gitglobal_p.h"
#include "observers/cloneobserver.h"
#include "observers/fetchobserver.h"
#include "observers/pushobserver.h"
#include "types.h"

#include "libkommit_debug.h"
#include <QFile>
#include <QProcess>

#include <git2.h>
#include <git2/branch.h>
#include <git2/diff.h>
#include <git2/errors.h>
#include <git2/refs.h>
#include <git2/repository.h>
#include <git2/stash.h>
#include <git2/submodule.h>
#include <git2/tag.h>

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

namespace Git
{

class ManagerPrivate
{
    Manager *q_ptr;
    Q_DECLARE_PUBLIC(Manager);

public:
    ManagerPrivate(Manager *parent);

    git_repository *repo{nullptr};

    QString path;
    bool isValid{false};

    int errorCode{};
    int errorClass{};
    QString errorMessage;

    CommitsCache *commitsCache;
    BranchesCache *branchesCache;
    TagsCache *tagsCache;
    RemotesCache *remotesCache;
    NotesCache *notesCache;
    SubmodulesCache *submodulesCache;
    StashesCache *stashesCache;
    ReferenceCache *referenceCache;

    void changeRepo(git_repository *repo);
    void resetCaches();

    void freeRepo();

    void checkError();
    static QHash<git_repository *, Manager *> managerMap;
};
QHash<git_repository *, Manager *> ManagerPrivate::managerMap;

const QString &Manager::path() const
{
    Q_D(const Manager);
    return d->path;
}

bool Manager::open(const QString &newPath)
{
    Q_D(Manager);

    if (d->path == newPath)
        return false;

    BEGIN
    git_repository *repo{};
    STEP git_repository_open_ext(&repo, newPath.toUtf8().data(), 0, NULL);

    END;

    d->commitsCache->clear();
    d->branchesCache->clear();
    d->tagsCache->clear();
    d->remotesCache->clear();
    d->notesCache->clear();
    d->submodulesCache->clear();
    d->stashesCache->clear();
    d->referenceCache->clear();

    d->isValid = IS_OK;

    if (IS_ERROR) {
        d->changeRepo(nullptr);
    } else {
        d->changeRepo(repo);
    }

    Q_EMIT pathChanged();
    Q_EMIT reloadRequired();

    return IS_OK;
}

QSharedPointer<Reference> Manager::head() const
{
    Q_D(const Manager);

    git_reference *head{nullptr};

    BEGIN
    STEP git_repository_head(&head, d->repo);

    if (IS_OK)
        return QSharedPointer<Reference>{new Reference{head}};
    return {};
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
    Q_D(const Manager);

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
        fs.setFullPath(d->path + QLatin1Char('/') + fs.name());
        files.append(fs);
    }
    return files;
}

bool Manager::isValid() const
{
    Q_D(const Manager);
    return d->isValid;
}

bool Manager::fetch(const QString &remoteName, FetchObserver *observer)
{
    Q_D(Manager);

    git_remote *remote;

    BEGIN

    STEP git_remote_lookup(&remote, d->repo, remoteName.toLocal8Bit().data());

    if (IS_ERROR) {
        END;
        return false;
    }

    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;

    if (observer) {
        observer->applyOfFetchOptions(&fetch_opts);
    }

    STEP git_remote_fetch(remote, NULL, &fetch_opts, "fetch");
    git_remote_free(remote);

    PRINT_ERROR;

    return IS_OK;
}

bool Manager::isIgnored(const QString &path)
{
    Q_D(Manager);

    BEGIN;
    int isIgnored;
    STEP git_ignore_path_is_ignored(&isIgnored, d->repo, path.toUtf8().data());

    if (IS_ERROR)
        return false;
    return !isIgnored;
}

QPair<int, int> Manager::uniqueCommitsOnBranches(const QString &branch1, const QString &branch2) const
{
    Q_D(const Manager);

    if (branch1 == branch2)
        return qMakePair(0, 0);

    size_t ahead;
    size_t behind;
    git_reference *ref1;
    git_reference *ref2;

    BEGIN
    STEP git_branch_lookup(&ref1, d->repo, branch1.toLocal8Bit().constData(), GIT_BRANCH_LOCAL);
    STEP git_branch_lookup(&ref2, d->repo, branch2.toLocal8Bit().constData(), GIT_BRANCH_LOCAL);

    if (IS_ERROR)
        return qMakePair(0, 0);
    auto id1 = git_reference_target(ref1);
    auto id2 = git_reference_target(ref2);
    STEP git_graph_ahead_behind(&ahead, &behind, d->repo, id1, id2);

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
    Q_D(const Manager);

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

    STEP git_revparse_single(&fromObject, d->repo, from.toLatin1().constData());
    STEP git_commit_lookup(&fromCommit, d->repo, git_object_id(fromObject));
    STEP git_commit_tree(&fromTree, fromCommit);

    if (to.isEmpty()) {
        git_diff_tree_to_workdir(&diff, d->repo, fromTree, &opts);
        RETURN_IF_ERR({});
    } else {
        STEP git_revparse_single(&toObject, d->repo, to.toLatin1().constData());
        STEP git_commit_lookup(&toCommit, d->repo, git_object_id(toObject));
        STEP git_commit_tree(&toTree, toCommit);

        RETURN_IF_ERR({});

        git_diff_tree_to_tree(&diff, d->repo, fromTree, toTree, &opts);
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
    Q_D(const Manager);

    BEGIN

    git_diff *diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;

    STEP git_diff_tree_to_tree(&diff, d->repo, from->tree().data(), to->tree().data(), &opts);

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
    Q_D(const Manager);

    if (!d->repo)
        return;

    git_revwalk *walker;
    git_commit *commit;
    git_oid oid;

    git_revwalk_new(&walker, d->repo);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);

    if (branchName.isEmpty()) {
        git_revwalk_push_head(walker);
    } else {
        git_reference *ref;
        auto n = git_branch_lookup(&ref, d->repo, branchName.toLocal8Bit().data(), GIT_BRANCH_ALL);

        if (n)
            return;

        auto refName = git_reference_name(ref);
        git_revwalk_push_ref(walker, refName);
    }

    while (!git_revwalk_next(&oid, walker)) {
        if (git_commit_lookup(&commit, d->repo, &oid)) {
            fprintf(stderr, "Failed to lookup the next object\n");
            return;
        }

        auto c = new Commit{commit};
        callback(QSharedPointer<Commit>{c});
    }

    git_revwalk_free(walker);
}

QSharedPointer<Index> Manager::index() const
{
    Q_D(const Manager);

    git_index *index;
    BEGIN
    STEP git_repository_index(&index, d->repo);
    PRINT_ERROR;

    if (IS_ERROR)
        return nullptr;

    return QSharedPointer<Index>{new Index{index}};
}

QSharedPointer<Tree> Manager::headTree() const
{
    Q_D(const Manager);

    git_reference *ref;
    git_tree *tree;

    BEGIN
    STEP git_repository_head(&ref, d->repo);
    PRINT_ERROR;
    RETURN_IF_ERR(nullptr);
    auto r = Reference{ref};
    auto oid = git_reference_target(ref);
    STEP git_tree_lookup(&tree, d->repo, oid);
    PRINT_ERROR;
    RETURN_IF_ERR(nullptr);

    return QSharedPointer<Tree>{new Tree{tree}};
}

TreeDiff Manager::diff(QSharedPointer<Tree> oldTree, QSharedPointer<Tree> newTree)
{
    Q_D(Manager);

    git_diff *diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;
    git_diff_stats *stats;

    BEGIN

    if (newTree) {
        STEP git_diff_tree_to_tree(&diff, d->repo, oldTree->gitTree(), newTree->gitTree(), &opts);
    } else {
        STEP git_diff_tree_to_workdir(&diff, d->repo, oldTree->gitTree(), &opts);
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
    Q_D(const Manager);

    if (!d->repo) {
        return {};
    }
    BEGIN
    git_config *cfg;
    switch (type) {
    case ConfigLocal:
        STEP git_repository_config(&cfg, d->repo);
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
    Q_D(const Manager);

    BEGIN
    int buf{};
    git_config *cfg;
    switch (type) {
    case ConfigLocal:
        STEP git_repository_config(&cfg, d->repo);
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
    Q_D(const Manager);

    BEGIN

    git_config *cfg;
    switch (type) {
    case ConfigLocal:
        STEP git_repository_config(&cfg, d->repo);
        break;
    case ConfigGlobal:
        STEP git_config_open_default(&cfg);
        break;
    }
    STEP git_config_set_string(cfg, name.toLatin1().data(), value.toLatin1().data());
    END;
}

void Manager::unsetConfig(const QString &name, ConfigType type) const
{
    Q_D(const Manager);

    BEGIN

    git_config *cfg;
    switch (type) {
    case ConfigLocal:
        STEP git_repository_config(&cfg, d->repo);
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

Manager::Manager()
    : QObject()
    , d_ptr{new ManagerPrivate{this}}
{
    git_libgit2_init();
}

Manager::Manager(git_repository *repo)
    : Manager()
{
    Q_D(Manager);
    git_libgit2_init();
    d->repo = repo;
    d->path = git_repository_workdir(d->repo);
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

Manager::~Manager()
{
    Q_D(Manager);
    if (d->repo)
        git_repository_free(d->repo);
    delete d;
}

bool Manager::switchBranch(const QString &branchName) const
{
    Q_D(const Manager);

    git_reference *branch{nullptr};
    git_object *treeish = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    BEGIN
    STEP git_branch_lookup(&branch, d->repo, branchName.toLocal8Bit().constData(), GIT_BRANCH_ALL);
    STEP git_revparse_single(&treeish, d->repo, branchName.toLocal8Bit().constData());
    STEP git_checkout_tree(d->repo, treeish, &opts);
    auto refName = git_reference_name(branch);
    STEP git_repository_set_head(d->repo, refName);

    git_reference_free(branch);
    return IS_OK;
}

QString Manager::run(const AbstractCommand &cmd) const
{
    return runGit(cmd.generateArgs());
}

bool Manager::init(const QString &path)
{
    Q_D(Manager);

    BEGIN
    git_repository_init_options initopts = {GIT_REPOSITORY_INIT_OPTIONS_VERSION, GIT_REPOSITORY_INIT_MKPATH};
    STEP git_repository_init_ext(&d->repo, path.toLatin1().data(), &initopts);

    if (IS_ERROR)
        return false;

    d->path = path;
    d->isValid = true;
    return true;
}

bool Manager::clone(const QString &url, const QString &localPath, CloneObserver *observer)
{
    Q_D(Manager);

    // TODO: free _repo
    git_clone_options opts = GIT_CLONE_OPTIONS_INIT;

    if (observer) {
        observer->applyOfFetchOptions(&opts.fetch_opts);

        // opts.checkout_opts.progress_cb = &CloneCallbacks::git_helper_checkout_progress_cb;
        // opts.checkout_opts.notify_cb = &CloneCallbacks::git_helper_checkout_notify_cb;
        // opts.checkout_opts.perfdata_cb = &CloneCallbacks::git_helper_checkout_perfdata_cb;

        observer->init(&opts.checkout_opts);

        opts.fetch_opts.callbacks.payload = observer;
    }

    BEGIN
    STEP git_clone(&d->repo, url.toLocal8Bit().constData(), localPath.toLocal8Bit().constData(), &opts);

    PRINT_ERROR;

    if (IS_ERROR)
        return false;

    d->path = git_repository_workdir(d->repo);
    return IS_OK;
}

QString Manager::runGit(const QStringList &args) const
{
    Q_D(const Manager); //    qCDebug(KOMMITLIB_LOG).noquote() << "Running: git " << args.join(" ");

    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments(args);
    p.setWorkingDirectory(d->path);
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
    Q_D(const Manager);

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
    STEP git_revparse_single(&placeObject, d->repo, place.toLatin1().constData());
    STEP git_commit_lookup(&commit, d->repo, git_object_id(placeObject));
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
    Q_D(const Manager);

    git_object *placeObject{nullptr};
    git_commit *commit{nullptr};
    git_tree *tree{nullptr};
    git_tree_entry *entry{nullptr};
    git_blob *blob{nullptr};

    BEGIN
    STEP git_revparse_single(&placeObject, d->repo, place.toLatin1().constData());
    STEP git_commit_lookup(&commit, d->repo, git_object_id(placeObject));
    STEP git_commit_tree(&tree, commit);

    STEP git_tree_entry_bypath(&entry, tree, fileName.toLocal8Bit().constData());
    STEP git_blob_lookup(&blob, d->repo, git_tree_entry_id(entry));

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

BlameData Manager::blame(QSharedPointer<File> file)
{
    Q_D(Manager);

    git_blame *blame;
    git_blame_options options;

    BEGIN
    STEP git_blame_options_init(&options, GIT_BLAME_OPTIONS_VERSION);
    STEP git_blame_file(&blame, d->repo, file->fileName().toUtf8().data(), &options);
    END;

    PRINT_ERROR;
    RETURN_IF_ERR({});

    BlameData b;

    auto lines = file->content().split(QLatin1Char('\n'));

    auto count = git_blame_get_hunk_count(blame);
    for (size_t i = 0; i < count; ++i) {
        auto hunk = git_blame_get_hunk_byindex(blame, i);

        BlameDataRow row{convertToString(&hunk->final_commit_id, 20),
                         lines.mid(hunk->final_start_line_number, hunk->lines_in_hunk).join(QLatin1Char('\n')),
                         QString{hunk->orig_path},

                         d->commitsCache->findByOid(&hunk->final_commit_id),
                         d->commitsCache->findByOid(&hunk->orig_commit_id),

                         QSharedPointer<Signature>{new Signature{hunk->orig_signature}},
                         QSharedPointer<Signature>{new Signature{hunk->final_signature}},

                         hunk->final_start_line_number,
                         hunk->orig_start_line_number};
        b.append(row);
    }
    git_blame_free(blame);

    return b;
}

bool Manager::revertFile(const QString &filePath) const
{
    Q_D(const Manager);

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;

    const auto s = filePath.toStdString();
    const char *paths[] = {s.c_str()};
    opts.paths.strings = (char **)paths;
    opts.paths.count = 1;

    opts.checkout_strategy = GIT_CHECKOUT_FORCE;

    BEGIN
    STEP git_checkout_head(d->repo, &opts);

    PRINT_ERROR;

    return IS_OK;
}

QMap<QString, ChangeStatus> Manager::changedFiles() const
{
    Q_D(const Manager);

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

    git_status_foreach_ext(d->repo, &opts, cb, &w);

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
    Q_D(const Manager);

    git_index *index{nullptr};
    git_tree *tree{nullptr};
    git_oid oid;

    BEGIN
    STEP git_repository_index(&index, d->repo);
    if (file.startsWith(QLatin1Char('/')))
        STEP git_index_add_bypath(index, toConstChars(file.mid(1)));
    else
        STEP git_index_add_bypath(index, toConstChars(file));
    STEP git_index_write_tree_to(&oid, index, d->repo);
    STEP git_tree_lookup(&tree, d->repo, &oid);
    STEP git_index_write(index);

    PRINT_ERROR;

    git_tree_free(tree);
    git_index_free(index);
}

bool Manager::removeFile(const QString &file, bool cached) const
{
    Q_D(const Manager);

    git_index *index = nullptr;
    git_oid oid;
    git_tree *tree{nullptr};

    BEGIN
    STEP git_repository_index(&index, d->repo);
    STEP git_index_read(index, true);
    STEP git_index_remove_bypath(index, file.toLocal8Bit().constData());
    STEP git_index_write_tree(&oid, index);
    STEP git_tree_lookup(&tree, d->repo, &oid);
    // STEP git_index_write(index);
    PRINT_ERROR;

    git_tree_free(tree);
    git_index_free(index);

    PRINT_ERROR;

    return IS_OK;
}

bool Manager::isMerging() const
{
    Q_D(const Manager);

    if (Q_UNLIKELY(!d->repo))
        return false;

    auto state = git_repository_state(d->repo);

    return state == GIT_REPOSITORY_STATE_MERGE;
}

bool Manager::isRebasing() const
{
    Q_D(const Manager);

    if (Q_UNLIKELY(!d->repo))
        return false;

    auto state = git_repository_state(d->repo);

    return state == GIT_REPOSITORY_STATE_REBASE || state == GIT_REPOSITORY_STATE_REBASE_INTERACTIVE || state == GIT_REPOSITORY_STATE_REBASE_MERGE;
}

bool Manager::isDetached() const
{
    Q_D(const Manager);

    if (Q_UNLIKELY(!d->repo))
        return false;
    return git_repository_head_detached(d->repo) == 1;
}

int Manager::errorClass() const
{
    Q_D(const Manager);
    return d->errorClass;
}

git_repository *Manager::repoPtr() const
{
    Q_D(const Manager);
    return d->repo;
}

Manager *Manager::owner(git_repository *repo)
{
    return ManagerPrivate::managerMap.value(repo, nullptr);
}

CommitsCache *Manager::commits() const
{
    Q_D(const Manager);
    return d->commitsCache;
}

SubmodulesCache *Manager::submodules() const
{
    Q_D(const Manager);
    return d->submodulesCache;
}

RemotesCache *Manager::remotes() const
{
    Q_D(const Manager);
    return d->remotesCache;
}

BranchesCache *Manager::branches() const
{
    Q_D(const Manager);
    return d->branchesCache;
}

TagsCache *Manager::tags() const
{
    Q_D(const Manager);
    return d->tagsCache;
}

NotesCache *Manager::notes() const
{
    Q_D(const Manager);
    return d->notesCache;
}

StashesCache *Manager::stashes() const
{
    Q_D(const Manager);
    return d->stashesCache;
}

ReferenceCache *Manager::references() const
{
    Q_D(const Manager);
    return d->referenceCache;
}

QString Manager::errorMessage() const
{
    return QString{git_error_last()->message};
}

int Manager::errorCode() const
{
    Q_D(const Manager);
    return d->errorCode;
}

ManagerPrivate::ManagerPrivate(Manager *parent)
    : q_ptr{parent}
    , commitsCache{new CommitsCache{parent}}
    , branchesCache{new BranchesCache{parent}}
    , tagsCache{new TagsCache{parent}}
    , remotesCache{new RemotesCache{parent}}
    , notesCache{new NotesCache{parent}}
    , submodulesCache{new SubmodulesCache{parent}}
    , stashesCache{new StashesCache(parent)}
    , referenceCache{new ReferenceCache{parent}}
{
}

void ManagerPrivate::changeRepo(git_repository *repo)
{
    Q_Q(Manager);

    freeRepo();

    if (repo) {
        this->repo = repo;
        managerMap.insert(repo, q);
        path = git_repository_workdir(repo);
    }

    isValid = repo;

    resetCaches();
}

void ManagerPrivate::resetCaches()
{
    commitsCache->clear();
    branchesCache->clear();
    tagsCache->clear();
    remotesCache->clear();
    notesCache->clear();
}

void ManagerPrivate::freeRepo()
{
    if (repo) {
        managerMap.remove(repo);
        git_repository_free(repo);
        repo = nullptr;
    }
}

void ManagerPrivate::checkError()
{
    auto __git_err = git_error_last();
    errorClass = __git_err->klass;
    errorMessage = QString{__git_err->message};
}

} // namespace Git

#include "moc_gitmanager.cpp"
