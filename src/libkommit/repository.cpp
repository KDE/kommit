// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "repository.h"

#include "abstractreference.h"
#include "blame.h"
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
#include "entities/blob.h"
#include "entities/branch.h"
#include "entities/commit.h"
#include "entities/file.h"
#include "entities/index.h"
#include "entities/note.h"
#include "entities/submodule.h"
#include "entities/tree.h"
#include "entities/treediff.h"
#include "fetchoptions.h"
#include "filestatus.h"
#include "gitglobal_p.h"
#include "options/blameoptions.h"
#include "options/checkoutoptions.h"
#include "options/cloneoptions.h"
#include "proxy.h"
#include "pushoptions.h"
#include "remotecallbacks.h"
#include "strarray.h"

#include "libkommit_debug.h"
#include <QFile>
#include <QProcess>
#include <QElapsedTimer>

#include <git2/branch.h>
#include <git2/diff.h>
#include <git2/errors.h>
#include <git2/refs.h>
#include <git2/repository.h>
#include <git2/revwalk.h>
#include <git2/stash.h>
#include <git2/diff.h>
#include <git2/submodule.h>
#include <git2/tag.h>

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

namespace Git
{

class RepositoryPrivate
{
    Repository *q_ptr;
    Q_DECLARE_PUBLIC(Repository)

public:
    RepositoryPrivate(Repository *parent);
    ~RepositoryPrivate();

    git_repository *repo{nullptr};

    QString path;
    bool isValid{false};

    int errorCode{};
    int errorClass{};
    QString errorMessage;

    Index index;

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
    static QHash<git_repository *, Repository *> managerMap;
};
QHash<git_repository *, Repository *> RepositoryPrivate::managerMap;

const QString &Repository::path() const
{
    Q_D(const Repository);
    return d->path;
}

bool Repository::open(const QString &newPath)
{
    Q_D(Repository);

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
    Q_EMIT currentBranchChanged();

    return IS_OK;
}

bool Repository::merge(const Branch &source, const CheckoutOptions &checkoutOptions, const MergeOptions &mergeOptions)
{
    Q_D(Repository);

    // git_reference *upstream_ref = NULL;
    git_annotated_commit *upstream_head = NULL;
    git_merge_analysis_t analysis;
    git_merge_preference_t preference;

    /* 1. resolve upstream reference */
    // int err = git_reference_lookup(&upstream_ref, repo, upstreamBranch);
    // if (err < 0)
    //     goto cleanup;

    /* 2. convert to annotated commit */
    SequenceRunner r;

    r.run(git_annotated_commit_from_ref, &upstream_head, d->repo, source.reference().constData());
    if (r.isError())
        return false;

    /* 3. merge analysis */
    const git_annotated_commit *heads[] = {upstream_head};

    r.run(git_merge_analysis, &analysis, &preference, d->repo, heads, 1);
    if (r.isError())
        return false;

    /* 4. decide what to do */
    if (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE) {
        /* nothing to do */
        return false;
    }

    if (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD) {
        /* fast-forward */
        git_reference *head_ref = NULL;
        git_reference *new_ref = NULL;
        const git_oid *target_oid = git_annotated_commit_id(upstream_head);

        r.run(git_repository_head, &head_ref, d->repo);
        if (r.isError())
            return false;

        r.run(git_reference_set_target, &new_ref, head_ref, target_oid, "fast-forward");

        git_reference_free(head_ref);
        git_reference_free(new_ref);

        if (r.isError())
            return false;

        /* update working tree */
        git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
        checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

        r.run(git_checkout_head, d->repo, &checkout_opts);
        return r.isSuccess();
    }

    /* 5. normal merge */
    if (analysis & GIT_MERGE_ANALYSIS_NORMAL) {
        git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
        git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

        mergeOptions.apply(&merge_opts);
        checkoutOptions.apply(&checkout_opts);

        checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE | GIT_CHECKOUT_RECREATE_MISSING;

        r.run(git_merge, d->repo, heads, 1, &merge_opts, &checkout_opts);
        return r.isSuccess();

        /*
         * IMPORTANT:
         * at this point:
         * - index may have conflicts
         * - no commit is created yet
         */

        /* caller must:
         * - check git_index_has_conflicts()
         * - create merge commit if clean
         */
    }

    return true;

    // cleanup:
    //     git_annotated_commit_free(upstream_head);
    //     git_reference_free(upstream_ref);

}

Index Repository::mergeBranches(Branch from, Branch to, const MergeOptions &mergeOptions)
{
    Q_D(Repository);

    git_oid oid;
    git_index *merged_index;

    SequenceRunner r;
    r.run(git_merge_base, &oid, d->repo, from.object().id().data(), to.object().id().data());

    if (r.isError())
        return {};

    auto baseCommit = d->commitsCache->findByOid(&oid);

    git_merge_options opts;
    mergeOptions.apply(&opts);
    // r.run(git_merge_trees, &merged_index, d->repo, baseCommit.tree().data(), to.tree().data(), from.tree().data(), opts);

    return Index{merged_index};
}

Reference Repository::head() const
{
    Q_D(const Repository);

    git_reference *head{nullptr};

    BEGIN
    STEP git_repository_head(&head, d->repo);
    END;

    if (IS_OK)
        return Reference{head};
    return Reference{};
}

bool Repository::checkout(Object target, CheckoutOptions *options)
{
    Q_D(Repository);

    git_checkout_options checkoutOptions;
    git_checkout_options_init(&checkoutOptions, GIT_CHECKOUT_OPTIONS_VERSION);
    checkoutOptions.checkout_strategy = GIT_CHECKOUT_SAFE;

    if (options)
        options->apply(&checkoutOptions);

    git_checkout_tree(d->repo, target.data(), &checkoutOptions);

    return true;
}

QMap<QString, ChangeStatus> Repository::changedFiles(const QString &hash) const
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

QStringList Repository::ignoredFiles() const
{
    return readAllNonEmptyOutput({QStringLiteral("check-ignore"), QStringLiteral("*")});
}

// TODO: remove this

bool Repository::isValid() const
{
    Q_D(const Repository);
    return d->isValid;
}

// bool Repository::fetch(const QString &remoteName, FetchObserver *observer)
// {
//     Q_D(Repository);

//     git_remote *remote;

//     BEGIN

//     STEP git_remote_lookup(&remote, d->repo, remoteName.toUtf8().data());

//     if (IS_ERROR) {
//         END;
//         return false;
//     }

//     git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;

//     if (observer) {
//         observer->applyOfFetchOptions(&fetch_opts);
//     }

//     STEP git_remote_fetch(remote, NULL, &fetch_opts, "fetch");
//     git_remote_free(remote);

//     PRINT_ERROR;

//     return IS_OK;
// }

// bool Repository::fetch(const Remote &remote, FetchOptions *options)
// {
//     if (!remote.isConnected() && !remote.connect(Git::Remote::Direction::Fetch, options->remoteCallbacks()))
//         return false;

//     git_fetch_options opts;
//     git_fetch_options_init(&opts, GIT_FETCH_OPTIONS_VERSION);

//     options->apply(&opts);

//     int ret;
//     if (options->branch().isNull()) {
//         StrArray refSpecs{1};
//         refSpecs.add(options->branch().refName());
//         ret = SequenceRunner::runSingle(git_remote_fetch, remote.remotePtr(), &refSpecs, &opts, "fetch");
//     } else {
//         ret = SequenceRunner::runSingle(git_remote_fetch, remote.remotePtr(), (const git_strarray *)NULL, &opts, "fetch");
//     }

//     return GIT_OK == ret;
// }

bool Repository::isIgnored(const QString &path)
{
    Q_D(Repository);

    BEGIN;
    int isIgnored;
    STEP git_ignore_path_is_ignored(&isIgnored, d->repo, path.toUtf8().data());

    if (IS_ERROR)
        return false;
    return isIgnored;
}

QPair<int, int> Repository::uniqueCommitsOnBranches(const QString &branch1, const QString &branch2) const
{
    Q_D(const Repository);

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


static int diff_file_cb(
    const git_diff_delta *delta,
    float progress,
    void *payload)
{
    const char *target = (const char *)payload;

    if ((delta->new_file.path && strcmp(delta->new_file.path, target) == 0) ||
        (delta->old_file.path && strcmp(delta->old_file.path, target) == 0))
        return 1; // stop iteration – file found

    return 0;
}

QList<Commit> Repository::fileLog(const QString &fileName) const
{
    Q_D(const Repository);
    QList<Commit> ret;

    auto commits = readAllNonEmptyOutput({QStringLiteral("log"), QStringLiteral("--format=format:%H"), QStringLiteral("--"), fileName});
    for (auto &c: commits)
        ret << d->commitsCache->find(c);

    return ret;
    QElapsedTimer timer;
    timer.start();
    git_revwalk *walker;
    git_oid oid;

    SequenceRunner r;
    r.run(git_revwalk_new, &walker, d->repo);
    r.run(git_revwalk_sorting, walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);

    git_reference *ref;
    git_branch_iterator *it;
    git_branch_t b;

    git_branch_iterator_new(&it, d->repo, GIT_BRANCH_ALL);

    while (!git_branch_next(&ref, &b, it)) {
        auto refname = git_reference_name(ref);
        git_revwalk_push_ref(walker, refname);
    }
    git_branch_iterator_free(it);

    if (r.isError())
        return {};

    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;
    git_strarray paths;
    paths.count = 1;
    QByteArray utf8Path = fileName.toUtf8();
    const char *path = utf8Path.constData();
    paths.strings = const_cast<char **>(&path);

    while (!git_revwalk_next(&oid, walker)) {
        git_commit *commit;
        r.run(git_commit_lookup, &commit, d->repo, &oid);

        if (r.isError())
            return {};

        auto parentsCount = git_commit_parentcount(commit);

        for (auto i = 0u; i < parentsCount; ++i) {
            git_commit *parentCommit;
            r.run(git_commit_parent, &parentCommit, commit, i);

            git_tree *commitTree;
            git_tree *parentCommitTree;
            r.run(git_commit_tree, &commitTree, commit);
            r.run(git_commit_tree, &parentCommitTree, parentCommit);
            git_diff *diff;
            r.run(git_diff_tree_to_tree, &diff, d->repo, parentCommitTree, commitTree, &opts);

            git_diff_stats *stats;
            git_diff_get_stats(&stats, diff);
            auto count = git_diff_stats_files_changed(stats);
            for (auto di = 0u; di < count; ++di) {
                auto delta = git_diff_get_delta(diff, di);
                if (fileName == delta->new_file.path || fileName == delta->old_file.path) {
                    auto hash = git_oid_tostr_s(&oid);

                    auto commitToAdd = d->commitsCache->find(hash);
                    ret << commitToAdd;
                }
            }
        }
    }

    qDebug() << Q_FUNC_INFO << timer.elapsed();
    return ret;

    // for (auto &commit : list) {
    //     for (auto const &parentHash : commit.parents()) {
    //         auto parent = find(parentHash);
    //         parent.addChild(commit.commitHash());
    //     commit.setReferences(manager->references()->findForCommit(commit));

    //     }
    // }

    // git_revwalk_free(walker);

    // QElapsedTimer timer;
    // timer.start();
    // auto commits = d->commitsCache->allCommits();
    // for (auto &commit : commits) {
    //     auto parentHashes = commit.parents();
    //     for (auto &parent : parentHashes) {
    //         auto parentCommit = d->commitsCache->find(parent);

    //         auto d = diff(parentCommit.tree(), commit.tree());

    //         if (d.contains(fileName)) {
    //             ret << commit;
    //             break;
    //         }
    //     }
    // }
    // return ret;
}

// QList<FileStatus> Repository::diffBranches(const QString &from, const QString &to) const
// {
//     Q_D(const Repository);

//     BEGIN

//     git_tree *fromTree{nullptr};
//     git_tree *toTree{nullptr};

//     git_object *fromObject;
//     git_commit *fromCommit = NULL;
//     git_object *toObject;
//     git_commit *toCommit = NULL;
//     git_diff *diff;
//     git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
//     opts.flags = GIT_DIFF_NORMAL;

//     STEP git_revparse_single(&fromObject, d->repo, from.toLatin1().constData());
//     STEP git_commit_lookup(&fromCommit, d->repo, git_object_id(fromObject));
//     STEP git_commit_tree(&fromTree, fromCommit);

//     if (to.isEmpty()) {
//         git_diff_tree_to_workdir(&diff, d->repo, fromTree, &opts);
//         RETURN_IF_ERR({});
//     } else {
//         STEP git_revparse_single(&toObject, d->repo, to.toLatin1().constData());
//         STEP git_commit_lookup(&toCommit, d->repo, git_object_id(toObject));
//         STEP git_commit_tree(&toTree, toCommit);

//         RETURN_IF_ERR({});

//         git_diff_tree_to_tree(&diff, d->repo, fromTree, toTree, &opts);
//         git_commit_free(toCommit);
//     }

//     git_commit_free(fromCommit);

//     git_diff_stats *stats;
//     git_diff_get_stats(&stats, diff);
//     auto n = git_diff_stats_files_changed(stats);
//     QList<FileStatus> files2;

//     for (size_t i = 0; i < n; ++i) {
//         auto delta = git_diff_get_delta(diff, i);
//         DiffDelta d{delta};

//         FileStatus fs;
//         fs.mName = delta->new_file.path;

//         switch (delta->status) {
//         case GIT_DELTA_UNMODIFIED:
//             fs.mStatus = FileStatus::Unmodified;
//             break;
//         case GIT_DELTA_ADDED:
//             fs.mStatus = FileStatus::Added;
//             break;
//         case GIT_DELTA_DELETED:
//             fs.mStatus = FileStatus::Removed;
//             break;
//         case GIT_DELTA_MODIFIED:
//             fs.mStatus = FileStatus::Modified;
//             break;
//         case GIT_DELTA_RENAMED:
//             fs.mStatus = FileStatus::Renamed;
//             break;
//         case GIT_DELTA_COPIED:
//             fs.mStatus = FileStatus::Copied;
//             break;
//         case GIT_DELTA_IGNORED:
//             fs.mStatus = FileStatus::Ignored;
//             break;
//         case GIT_DELTA_UNTRACKED:
//             fs.mStatus = FileStatus::Untracked;
//             break;
//         case GIT_DELTA_TYPECHANGE:
//             fs.mStatus = FileStatus::Unknown;
//             break;
//         case GIT_DELTA_UNREADABLE:
//             fs.mStatus = FileStatus::Unknown;
//             break;
//         case GIT_DELTA_CONFLICTED:
//             fs.mStatus = FileStatus::Unknown;
//             break;
//         }
//         files2 << fs;
//     }

//     return files2;
// }

QList<DiffDelta> Repository::diff(AbstractReference *from, AbstractReference *to) const
{
    Q_D(const Repository);

    git_diff *diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;

    auto ok = SequenceRunner::runSingle(git_diff_tree_to_tree, &diff, d->repo, from->tree().data(), to->tree().data(), &opts);

    if (!ok)
        return QList<DiffDelta>{};

    git_diff_stats *stats;
    git_diff_get_stats(&stats, diff);
    auto n = git_diff_stats_files_changed(stats);
    QList<DiffDelta> files2;

    for (size_t i = 0; i < n; ++i) {
        auto delta = git_diff_get_delta(diff, i);
        files2 << DiffDelta{delta};
        /*
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
                */
    }

    return files2;
}

void Repository::forEachCommits(std::function<void(QSharedPointer<Commit>)> callback, const QString &branchName) const
{
    Q_D(const Repository);

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

Index &Repository::index()
{
    Q_D(Repository);

    if (d->index.isNull()) {
        git_index *index;
        auto ok = SequenceRunner::runSingle(git_repository_index, &index, d->repo);
        if (ok)
            d->index = Index{index};
    }

    return d->index;
}

Tree Repository::headTree() const
{
    Q_D(const Repository);

    git_reference *ref;
    git_commit *headCommit = nullptr;

    SequenceRunner r;

    r.run(git_repository_head, &ref, d->repo);
    r.run(git_reference_peel, (git_object **)&headCommit, ref, GIT_OBJ_COMMIT);

    if (r.isError())
        return Tree{};

    return Commit{headCommit}.tree();
}

TreeDiff Repository::diff(const Tree &oldTree, const Tree &newTree) const
{
    Q_D(const Repository);

    git_diff *diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_NORMAL;
    git_diff_stats *stats;

    BEGIN

    if (newTree.isNull()) {
        STEP git_diff_tree_to_workdir(&diff, d->repo, oldTree.data(), &opts);
    } else {
        STEP git_diff_tree_to_tree(&diff, d->repo, oldTree.data(), newTree.data(), &opts);
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

QStringList Repository::readAllNonEmptyOutput(const QStringList &cmd) const
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

Repository::Repository(QObject *parent)
    : QObject{parent}
    , d_ptr{new RepositoryPrivate{this}}
{
    git_libgit2_init();
}

Repository::Repository(git_repository *repo)
    : Repository{}
{
    Q_D(Repository);

    d->changeRepo(repo);
}

Repository::Repository(const QString &path)
    : Repository{}
{
    (void)open(path);
}

Repository::~Repository()
{
}

Repository *Repository::instance()
{
    static Repository instance;
    return &instance;
}

bool Repository::setHead(const Reference &ref) const
{
    Q_D(const Repository);
    BEGIN;
    STEP git_repository_set_head(d->repo, ref.name().toUtf8().data());
    END;
    return IS_OK;
}

bool Repository::reset(const Commit &commit, ResetType type) const
{
    Q_D(const Repository);
    git_object *object;
    SequenceRunner r;
    r.run(git_object_lookup, &object, d->repo, commit.oid().data(), GIT_OBJECT_COMMIT);

    if (r.isError())
        return false;

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    r.run(git_reset, d->repo, object, static_cast<git_reset_t>(type), &opts);
    r.printError();

    if (r.isSuccess())
        Q_EMIT currentBranchChanged();

    return r.isSuccess();
}

bool Repository::fetch(Remote remote, Branch branch, FetchOptions *options)
{
    if (remote.isNull())
        return false;


    if (!remote.isConnected() && !remote.connect(Git::Remote::Direction::Fetch, options->remoteCallbacks()))
        return false;

    git_fetch_options opts = GIT_FETCH_OPTIONS_INIT;

    options->apply(&opts);

    int ret;
    if (!branch.isNull()) {
        StrArray refSpecs{1};
        refSpecs.add(branch.refName());
        ret = SequenceRunner::runSingle(git_remote_fetch, remote.data(), &refSpecs, &opts, "fetch");
    } else {
        ret = SequenceRunner::runSingle(git_remote_fetch, remote.data(), (const git_strarray *)NULL, &opts, "fetch");
    }

    // emit options->finished(ret);
    return ret;
}

Config Repository::config() const
{
    Q_D(const Repository);

    git_config *cfg;
    if (git_repository_config(&cfg, d->repo))
        return Config{nullptr};

    return Config{cfg};
}

Config Repository::globalConfig()
{
    git_config *cfg;
    if (git_config_open_default(&cfg))
        return Config{nullptr};
    return Config{cfg};
}

QString Repository::run(const AbstractCommand &cmd) const
{
    return runGit(cmd.generateArgs());
}

bool Repository::init(const QString &path, InitOptions *options)
{
    Q_D(Repository);
    git_repository *repo;

    git_repository_init_options initopts = {GIT_REPOSITORY_INIT_OPTIONS_VERSION, GIT_REPOSITORY_INIT_MKPATH};
    if (options)
        options->apply(&initopts);

    auto ok = SequenceRunner::runSingle(git_repository_init_ext, &repo, path.toUtf8().data(), &initopts);

    if (!ok) {
        git_repository_free(repo);
        return false;
    }

    d->changeRepo(repo);
    d->path = path;
    d->isValid = true;
    return true;
}

bool Repository::clone(const QString &url, const QString &localPath, CloneOptions *options)
{
    Q_D(Repository);

    git_clone_options opts = GIT_CLONE_OPTIONS_INIT;

    if (options)
        options->apply(&opts);
    git_repository *repo{nullptr};

    auto ok = SequenceRunner::runSingle(git_clone, &repo, url.toStdString().c_str(), localPath.toStdString().c_str(), &opts);

    if (!ok) {
        git_repository_free(repo);
        return false;
    }

    d->changeRepo(repo);
    return true;
}

QString Repository::runGit(const QStringList &args) const
{
    Q_D(const Repository); //    qCDebug(KOMMITLIB_LOG).noquote() << "Running: git " << args.join(" ");

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

QStringList Repository::ls(const QString &place) const
{
    Q_D(const Repository);

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

QString Repository::fileContent(const QString &place, const QString &fileName) const
{
    Q_D(const Repository);

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

Blame Repository::blame(const QString &filePath, BlameOptions *options)
{
    Q_D(Repository);

    git_blame *blame;
    git_blame_options opts;

    QFile file{d->path + QStringLiteral("/") + filePath};
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return Blame{};

    auto content = QString{file.readAll()};
    file.close();

    SequenceRunner r;
    r.run(git_blame_options_init, &opts, GIT_BLAME_OPTIONS_VERSION);
    if (options)
        options->apply(&opts);

    r.run(git_blame_file, &blame, d->repo, filePath.toUtf8().data(), &opts);

    if (r.isError())
        return Blame{};

    return Blame{this, content.split('\n'), blame};
}

QList<FileStatus> Repository::changedFiles(StatusOptions options) const
{
    Q_D(const Repository);

    git_status_options *opts;
    options.apply(opts);
    git_status_list *list;
    git_status_list_new(&list, d->repo, opts);

    auto count = git_status_list_entrycount(list);
    QList<FileStatus> files;
    for (auto i = 0; i < count; ++i) {
        auto entry = git_status_byindex(list, i);

        auto statusFlags = static_cast<StatusFlags>(entry->status);
        DiffDelta headToIndex{entry->head_to_index};
        DiffDelta indexToWorkdir{entry->index_to_workdir};

        files << FileStatus{statusFlags, headToIndex, indexToWorkdir};
    }
    return files;
}

QMap<QString, DeltaFlag> Repository::changedFiles(const Commit &commit) const
{
    QMap<QString, DeltaFlag> ret;
    auto parents = commit.parents();
    for (auto &parent : parents) {
        auto parentCommit = commits()->find(parent);
        auto treeDiff = diff(parentCommit.tree(), commit.tree());

        for (TreeDiffEntry &d : treeDiff) {
            // auto status = toChangeStatus(d.status());

            if (ret.contains(d.newFile())) {
            }

            ret.insert(d.newFile(), d.status());
        }
    }
    return ret;
}

bool Repository::revertFile(const QString &filePath) const
{
    Q_D(const Repository);

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

QMap<QString, StatusFlags> Repository::changedFiles() const
{
    Q_D(const Repository);

    // git_status_list *list;
    // git_status_list_new(&list, d->repo, nullptr);

    // auto count = git_status_list_entrycount(list);
    // QList<FileStatus> files;
    // for (auto i = 0; i < count; ++i) {
    //     auto entry = git_status_byindex(list, i);

    //     auto statusFlags = static_cast<StatusFlags>(entry->status);
    //     DiffDelta headToIndex{entry->head_to_index};
    //     DiffDelta indexToWorkdir{entry->index_to_workdir};

    //     fiels << FileStatus{statusFlags, headToIndex, indexToWorkdir};
    //     // entry->
    // }

    struct wrapper {
        QMap<QString, StatusFlags> files;
        QList<FileStatus> filsList;
    };
    auto cb = [](const char *path, unsigned int status_flags, void *payload) -> int {
        auto w = reinterpret_cast<wrapper *>(payload);
        auto status = static_cast<StatusFlags>(status_flags);

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

bool Repository::commit(const QString &message, Branch branch, const CommitOptions &options)
{
    Q_D(Repository);
    SequenceRunner r;
    git_oid commit_oid;
    git_tree *tree = nullptr;
    git_commit *parent_commit = nullptr;

    auto indexPtr = this->index();
    indexPtr.writeTree();
    auto lastTreeId = indexPtr.lastOid();

    // r.run(git_repository_index, &index, d->repo);
    // r.run(git_index_write_tree, &tree_oid, index);
    r.run(git_tree_lookup, &tree, d->repo, &lastTreeId);

    git_reference *head_ref = nullptr;
    git_oid parent_commit_oid;

    if (branch.isNull()) {
        branch = branches()->current();
    } else {
        // commit_oid = *branch.commit().oid().data();
    }

    if (git_repository_head(&head_ref, d->repo) == 0) {
        r.run(git_reference_name_to_id, &parent_commit_oid, d->repo, "HEAD");
        r.run(git_commit_lookup, &parent_commit, d->repo, &parent_commit_oid);
    } else {
        qDebug() << "No head found";
    }

    auto reflogMessage = options.reflogMessage();

    if (reflogMessage.isEmpty())
        reflogMessage = QStringLiteral("refs/heads/main");

    git_commit *parents[1] = {const_cast<git_commit *>(parent_commit)};

    git_signature *author;
    git_signature *committer;
    if (options.author().isNull())
        git_signature_default(&author, d->repo);
    else
        author = options.author().data();

    if (options.committer().isNull())
        git_signature_default(&committer, d->repo);
    else
        committer = options.committer().data();
    git_commit_create_options opts;
    opts = GIT_COMMIT_CREATE_OPTIONS_INIT;
    // r.run(git_commit_create_from_stage, &commit_oid, d->repo, message.toUtf8().constData(), &opts);
    r.run(git_commit_create_v, &commit_oid, d->repo, "HEAD", author, committer, nullptr, message.toUtf8().constData(), tree, 0);

    git_tree_free(tree);
    git_commit_free(parent_commit);
    git_reference_free(head_ref);

    d->index = Index{};
    Q_EMIT reloadRequired();
    return r.isSuccess();
}

bool Repository::pull(const Remote &remote,
                      const Branch &branch,
                      const FetchOptions &fetchOptions,
                      const CheckoutOptions &checkoutOptions,
                      const MergeOptions &mergeOptions)
{
    Q_D(const Repository);

    auto upstreamBranch = d->branchesCache->findByName(branch.upStreamName());

    if (upstreamBranch.isNull()) {
        auto upstreamGuessName = remote.name() + "/" + branch.name();
        upstreamBranch = d->branchesCache->findByName(upstreamGuessName);
    }

    if (upstreamBranch.isNull()) {
        // unable to guess the upstream branch
        return false;
    }

    // fetchOptions.setBranch(branch);
    // fetchOptions.setRemote(remote);
    // if (!fetch(&fetchOptions))
        // return false;
    if (!merge(upstreamBranch, checkoutOptions, mergeOptions))
        return false;
    return true;
}

bool Repository::push(const Branch &branch, const Remote &remote, PushOptions *options)
{
    git_push_options opts = GIT_PUSH_OPTIONS_INIT;
    if (options)
        options->apply(&opts);

    StrArray a{branch.refName() + ":" + branch.refName()};
    auto ok = SequenceRunner::runSingle(git_remote_push, remote.data(), &a, &opts);

    return ok;
}

bool Repository::isMerging() const
{
    Q_D(const Repository);

    if (Q_UNLIKELY(!d->repo))
        return false;

    auto state = git_repository_state(d->repo);

    return state == GIT_REPOSITORY_STATE_MERGE;
}

bool Repository::switchBranch(const Branch &branch) const
{
    if (branch.isNull()) {
        qDebug() << "Branch is null";
        return false;
    }
    Q_D(const Repository);

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    auto ref = branch.data();

    git_object *target = nullptr;

    SequenceRunner r;

    r.run(git_reference_peel, &target, ref, GIT_OBJ_COMMIT);
    r.run(git_checkout_tree, d->repo, target, &opts);
    r.run(git_repository_set_head, d->repo, branch.refName().toUtf8().constData());

    return r.isSuccess();
}

bool Repository::switchBranch(const QString &branchName) const
{
    Q_D(const Repository);

    git_reference *branch{nullptr};
    git_object *treeish = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;

    opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    SequenceRunner r;
    r.run(git_branch_lookup, &branch, d->repo, branchName.toLocal8Bit().constData(), GIT_BRANCH_ALL);
    r.run(git_reference_resolve, &branch, branch);
    r.run(git_revparse_single, &treeish, d->repo, branchName.toLocal8Bit().constData());
    r.run(git_checkout_tree, d->repo, treeish, &opts);
    auto refName = git_reference_name(branch);
    r.run(git_repository_set_head, d->repo, refName);

    if (r.isSuccess())
        Q_EMIT currentBranchChanged();

    git_reference_free(branch);
    return r.isSuccess();
}

bool Repository::isRebasing() const
{
    Q_D(const Repository);

    if (Q_UNLIKELY(!d->repo))
        return false;

    auto state = git_repository_state(d->repo);

    return state == GIT_REPOSITORY_STATE_REBASE || state == GIT_REPOSITORY_STATE_REBASE_INTERACTIVE || state == GIT_REPOSITORY_STATE_REBASE_MERGE;
}

bool Repository::isDetached() const
{
    Q_D(const Repository);

    if (Q_UNLIKELY(!d->repo))
        return false;
    return git_repository_head_detached(d->repo) == 1;
}

int Repository::errorClass() const
{
    Q_D(const Repository);
    return d->errorClass;
}

git_repository *Repository::repoPtr() const
{
    Q_D(const Repository);
    return d->repo;
}

Repository *Repository::owner(git_repository *repo)
{
    auto ret = RepositoryPrivate::managerMap.value(repo, nullptr);
    Q_ASSERT(ret);
    return ret;
}

CommitsCache *Repository::commits() const
{
    Q_D(const Repository);
    return d->commitsCache;
}

SubmodulesCache *Repository::submodules() const
{
    Q_D(const Repository);
    return d->submodulesCache;
}

RemotesCache *Repository::remotes() const
{
    Q_D(const Repository);
    return d->remotesCache;
}

BranchesCache *Repository::branches() const
{
    Q_D(const Repository);
    return d->branchesCache;
}

TagsCache *Repository::tags() const
{
    Q_D(const Repository);
    return d->tagsCache;
}

NotesCache *Repository::notes() const
{
    Q_D(const Repository);
    return d->notesCache;
}

StashesCache *Repository::stashes() const
{
    Q_D(const Repository);
    return d->stashesCache;
}

ReferenceCache *Repository::references() const
{
    Q_D(const Repository);
    return d->referenceCache;
}

QString Repository::errorMessage() const
{
    return QString{git_error_last()->message};
}

int Repository::errorCode() const
{
    Q_D(const Repository);
    return d->errorCode;
}

RepositoryPrivate::RepositoryPrivate(Repository *parent)
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

RepositoryPrivate::~RepositoryPrivate()
{
    if (repo)
        git_repository_free(repo);
}

void RepositoryPrivate::changeRepo(git_repository *repo)
{
    Q_Q(Repository);

    if (repo == this->repo)
        return;

    freeRepo();

    if (repo) {
        this->repo = repo;
        managerMap.insert(repo, q);
        path = git_repository_workdir(repo);
    }

    isValid = repo;

    resetCaches();
}

void RepositoryPrivate::resetCaches()
{
    commitsCache->clear();
    branchesCache->clear();
    tagsCache->clear();
    remotesCache->clear();
    notesCache->clear();
    submodulesCache->clear();
    stashesCache->clear();
    referenceCache->clear();
}

void RepositoryPrivate::freeRepo()
{
    if (repo) {
        managerMap.remove(repo);
        git_repository_free(repo);
        repo = nullptr;
    }
}

void RepositoryPrivate::checkError()
{
    auto __git_err = git_error_last();
    errorClass = __git_err->klass;
    errorMessage = QString{__git_err->message};
}

} // namespace Git

#include "diffdelta.h"
#include "moc_repository.cpp"

#include <checkoutoptions.h>
