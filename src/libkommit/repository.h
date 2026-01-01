// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "libkommit_export.h"

#include "diffdelta.h"
#include "libkommit_global.h"
#include "types.h"

#include <git2.h>
#include <statusoptions.h>

#include <Kommit/Blame>
#include <Kommit/Branch>
#include <Kommit/CommitOptions>
#include <Kommit/Config>
#include <Kommit/InitOptions>
#include <Kommit/MergeOptions>

#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>

namespace Git
{

class Tag;
class Repository;
class Submodule;
class Reference;
class AbstractReference;
class Index;
class Tree;
class Note;
class RepositoryPrivate;
class Commit;
class CommitsCache;
class BranchesCache;
class TagsCache;
class RemotesCache;
class NotesCache;
class SubmodulesCache;
class StashesCache;
class ReferenceCache;
class AbstractCommand;
class FileStatus;
class File;
class TreeDiff;
struct BlameDataRow;
class BlameOptions;
class Blame;
class BlameData;
class RemoteCallbacks;
class FetchOptions;
class CloneOptions;
class PushOptions;
class CheckoutOptions;

class LIBKOMMIT_EXPORT Repository : public QObject
{
    Q_OBJECT

public:
    enum class BranchType {
        LocalBranch,
        RemoteBranch,
        AllBranches,
    };
    enum ConfigType {
        ConfigGlobal,
        ConfigLocal,
    };
    enum class ResetType {
        Soft = GIT_RESET_SOFT,
        Mied = GIT_RESET_MIXED,
        Hard = GIT_RESET_HARD
    };

    explicit Repository(QObject *parent = nullptr);
    explicit Repository(git_repository *repo);
    explicit Repository(const QString &path);
    ~Repository();
    static Repository *instance();

    // run
    QString run(const AbstractCommand &cmd) const;
    QString runGit(const QStringList &args) const;

    // common actions
    bool init(const QString &path, InitOptions *options = nullptr);
    bool clone(const QString &url, const QString &localPath, CloneOptions *options = nullptr);
    bool commit(const QString &message, Branch branch = {}, const CommitOptions &options = {});
    bool pull(const Remote &remote,
              const Branch &branch,
              const FetchOptions &fetchOptions,
              const CheckoutOptions &checkoutOptions,
              const MergeOptions &mergeOptions);
    bool push(const Branch &branch, const Remote &remote, const QString &upstreamRefName = {}, bool force = false, PushOptions *options = nullptr);
    bool open(const QString &newPath);
    MergeAnalysis mergeAnalyse(QList<AnnotatedCommit> commits);
    bool merge(const Branch &source, const CheckoutOptions &checkoutOptions, const MergeOptions &mergeOptions);
    Index mergeBranches(Branch from, Branch to, const MergeOptions &mergeOptions);
    Reference head() const;
    bool checkout(Object target, CheckoutOptions *options = nullptr);

    // properties
    [[nodiscard]] const QString &path() const;
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isMerging() const;

    // branches
    bool switchBranch(const Branch &branch) const;
    bool switchBranch(const QString &branchName) const;
    [[nodiscard]] QPair<int, int> uniqueCommitsOnBranches(const QString &branch1, const QString &branch2) const;
    bool setHead(const Reference &ref) const;
    bool reset(const Commit &commit, ResetType type) const;

    // remotes
    bool fetch(Remote remote, Branch branch, FetchOptions *options);

    // config
    Config config() const;
    static Config globalConfig();

    // files
    [[nodiscard]] QStringList ls(const QString &place) const;
    [[nodiscard]] QString fileContent(const QString &place, const QString &fileName) const;
    bool revertFile(const QString &filePath) const;
    [[nodiscard]] QList<Commit> fileLog(const QString &fileName) const;
    Blame blame(const QString &filePath, BlameOptions *options = nullptr);

    [[nodiscard]] QMap<QString, StatusFlags> changedFiles() const;
    [[nodiscard]] QList<FileStatus> changedFiles(StatusOptions options) const;
    [[nodiscard]] QMap<QString, DeltaFlag> changedFiles(const Commit &commit) const;
    [[nodiscard]] QMap<QString, ChangeStatus> changedFiles(const QString &hash) const;
    [[nodiscard]] QStringList ignoredFiles() const;

    // ignores
    bool isIgnored(const QString &path);

    // diffs
    // [[nodiscard]] QList<FileStatus> diffBranches(const QString &from, const QString &to) const;
    [[nodiscard]] QList<DiffDelta> diff(AbstractReference *from, AbstractReference *to) const;
    [[nodiscard]] TreeDiff diff(const Tree &oldTree, const Tree &newTree) const;

    Q_DECL_DEPRECATED_X("Use commits()->forEach")
    void forEachCommits(std::function<void(QSharedPointer<Commit>)> callback, const QString &branchName) const;

    [[nodiscard]] Index &index();
    [[nodiscard]] Tree headTree() const;

    [[nodiscard]] bool isRebasing() const;
    [[nodiscard]] bool isDetached() const;

    [[nodiscard]] int errorCode() const;

    [[nodiscard]] QString errorMessage() const;

    [[nodiscard]] int errorClass() const;

    [[nodiscard]] git_repository *repoPtr() const;
    [[nodiscard]] static Repository *owner(git_repository *repo);

    [[nodiscard]] CommitsCache *commits() const;
    [[nodiscard]] SubmodulesCache *submodules() const;
    [[nodiscard]] RemotesCache *remotes() const;
    [[nodiscard]] BranchesCache *branches() const;
    [[nodiscard]] TagsCache *tags() const;
    [[nodiscard]] NotesCache *notes() const;
    [[nodiscard]] StashesCache *stashes() const;
    [[nodiscard]] ReferenceCache *references() const;

Q_SIGNALS:
    void pathChanged();
    void reloadRequired();
    void currentBranchChanged() const;

private:
    QScopedPointer<RepositoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Repository)
    LIBKOMMIT_NO_EXPORT QStringList readAllNonEmptyOutput(const QStringList &cmd) const;
};

} // namespace Git
