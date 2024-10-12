// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "libkommit_export.h"

#include "libkommit_global.h"
#include "types.h"

#include <git2.h>

#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>

namespace Git
{

class Branch;
class Tag;
class Repository;
class Submodule;
class FetchObserver;
class CloneObserver;
class PushObserver;
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
class Blob;
struct BlameDataRow;
class BlameOptions;
class BlameData;

// TODO: rename class and file's name to repository
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
    enum class ResetType { Soft = GIT_RESET_SOFT, Mied = GIT_RESET_MIXED, Hard = GIT_RESET_HARD };

    explicit Repository(QObject *parent = nullptr);
    explicit Repository(git_repository *repo);
    explicit Repository(const QString &path);
    ~Repository();
    static Repository *instance();

    // run
    QString run(const AbstractCommand &cmd) const;
    QString runGit(const QStringList &args) const;

    // common actions
    bool init(const QString &path);
    bool clone(const QString &url, const QString &localPath, CloneObserver *observer = nullptr);
    bool commit(const QString &message);
    void push(PushObserver *observer = nullptr) const;
    bool open(const QString &newPath);
    QSharedPointer<Reference> head() const;
    bool checkout();

    // properties
    [[nodiscard]] const QString &path() const;
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isMerging() const;

    // branches
    bool switchBranch(QSharedPointer<Branch> branch) const;
    bool switchBranch(const QString &branchName) const;
    [[nodiscard]] QPair<int, int> uniqueCommitsOnBranches(const QString &branch1, const QString &branch2) const;
    bool setHead(QSharedPointer<Reference> ref) const;
    bool reset(QSharedPointer<Commit> commit, ResetType type) const;

    // remotes
    bool fetch(const QString &remoteName, FetchObserver *observer = nullptr);

    // config
    [[nodiscard]] QString config(const QString &name, ConfigType type = ConfigLocal) const;
    [[nodiscard]] bool configBool(const QString &name, ConfigType type = ConfigLocal) const;
    void setConfig(const QString &name, const QString &value, ConfigType type = ConfigLocal) const;
    void unsetConfig(const QString &name, ConfigType type = ConfigLocal) const;
    void forEachConfig(std::function<void(QString, QString)> cb);

    // files
    void addFile(const QString &file);
    [[nodiscard]] QStringList ls(const QString &place) const;
    [[nodiscard]] QString fileContent(const QString &place, const QString &fileName) const;
    void saveFile(const QString &place, const QString &fileName, const QString &localFile) const;
    bool revertFile(const QString &filePath) const;
    bool removeFile(const QString &file, bool cached) const;
    [[nodiscard]] QStringList fileLog(const QString &fileName) const;
    QSharedPointer<BlameData> blame(const QString &filePath, BlameOptions *options = nullptr);
    [[nodiscard]] QMap<QString, ChangeStatus> changedFiles() const;
    [[nodiscard]] QMap<QString, ChangeStatus> changedFiles(const QString &hash) const;
    [[nodiscard]] QStringList ignoredFiles() const;

    [[nodiscard]] Q_DECL_DEPRECATED QList<FileStatus> repoFilesStatus() const;

    // ignores
    bool isIgnored(const QString &path);

    // diffs
    [[nodiscard]] QString diff(const QString &from, const QString &to) const;
    [[nodiscard]] QList<FileStatus> diffBranch(const QString &from) const;
    [[nodiscard]] QList<FileStatus> diffBranches(const QString &from, const QString &to) const;
    [[nodiscard]] QList<FileStatus> diff(AbstractReference *from, AbstractReference *to) const;
    [[nodiscard]] TreeDiff diff(QSharedPointer<Tree> oldTree, QSharedPointer<Tree> newTree = {});

    void forEachCommits(std::function<void(QSharedPointer<Commit>)> callback, const QString &branchName) const;

    [[nodiscard]] QSharedPointer<Index> index();
    [[nodiscard]] QSharedPointer<Tree> headTree() const;

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
